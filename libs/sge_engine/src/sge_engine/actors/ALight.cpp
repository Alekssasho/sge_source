#include "ALight.h"
#include "sge_engine/GameWorld.h"

#include "sge_engine/typelibHelper.h"

namespace sge {


// clang-format off
DefineTypeId(LightType, 20'03'01'0015);
DefineTypeId(LightDesc, 20'03'01'0016);
DefineTypeId(ALight, 20'03'01'0017);

ReflBlock() {
	ReflAddType(LightType)
		ReflEnumVal(light_point, "light_point")
		ReflEnumVal(light_directional, "light_directional")
		ReflEnumVal(light_spot, "light_spot")
	;

	ReflAddType(LightDesc) ReflMember(LightDesc, isOn)
		ReflMember(LightDesc, type)
		ReflMember(LightDesc, intensity).uiRange(0.01f, 10000.f, 0.01f)
		ReflMember(LightDesc, range).uiRange(0.01f, 10000.f, 0.5f)
		ReflMember(LightDesc, color)
	        .addMemberFlag(MFF_Vec3fAsColor) ReflMember(LightDesc, spotLightAngle)
	        .addMemberFlag(MFF_FloatAsDegrees) ReflMember(LightDesc, hasShadows)
		ReflMember(LightDesc, shadowMapRes)
	;

	ReflAddActor(ALight)
		ReflMember(ALight, m_lightDesc);
}
// clang-format on


Optional<ShadowMapBuildInfo> LightDesc::buildShadowMapInfo(const transf3d& lightWs, const Frustum& mainCameraFrustumWs) const {
	// Check if the light could have shadows, if not just return an empty structure.
	if (isOn == false || hasShadows == false) {
		return NullOptional();
	}

	switch (type) {
		case light_directional: {
			vec3f mainCameraFrustumCornersWs[8];
			mainCameraFrustumWs.getCorners(mainCameraFrustumCornersWs, range);

			transf3d lightToWsNoScaling = lightWs;
			lightToWsNoScaling.s = vec3f(1.f);

			const mat4f ls2ws = lightToWsNoScaling.toMatrix();
			const mat4f ws2ls = inverse(ls2ws);
			AABox3f frustumLSBBox;
			for (int t = 0; t < SGE_ARRSZ(mainCameraFrustumCornersWs); ++t) {
				frustumLSBBox.expand(mat_mul_pos(ws2ls, mainCameraFrustumCornersWs[t]));
			}

			// Expand the bbox a in the direction opposite of the light.
			// This is needed in order to still render the objects that are a little bit behind the camera, so they could still cast
			// shadows.
			// frustumLSBBox.expand(frustumLSBBox.min - frustumLSBBox.size().x00() * 0.1f);

			// Make the bbox a bit wider, so objects near the frustum could still cast shadows.
			// frustumLSBBox.scaleAroundCenter(vec3f(1.f, 1.1f, 1.1f));

			const vec3f camPosLs = frustumLSBBox.center() - frustumLSBBox.halfDiagonal().x00();
			const vec3f camPosWs = mat_mul_pos(ls2ws, camPosLs);

			const float camWidth = frustumLSBBox.size().z;
			const float camheight = frustumLSBBox.size().y;
			const float camZRange = frustumLSBBox.size().x;

			transf3d shadowCameraTrasnform = lightToWsNoScaling;
			shadowCameraTrasnform.p = camPosWs;

			const mat4f shadowViewMtx = mat4f::getRotationY(half_pi()) * shadowCameraTrasnform.toMatrix().inverse();
			const mat4f shadowProjMtx = mat4f::getOrthoRHCentered(camWidth, camheight, 0.f, camZRange, kIsTexcoordStyleD3D);
			const RawCamera shadowMapCamera = RawCamera(camPosWs, shadowViewMtx, shadowProjMtx);

			return ShadowMapBuildInfo(shadowMapCamera);
		} break;

		case light_spot: {
			transf3d lightToWsNoScaling = lightWs;
			lightToWsNoScaling.s = vec3f(1.f);

			const mat4f shadowViewMtx = mat4f::getRotationY(half_pi()) * lightToWsNoScaling.toMatrix().inverse();
			const mat4f shadowProjMtx = mat4f::getPerspectiveFovRH(spotLightAngle * 2.f, 1.f, 0.1f, range, kIsTexcoordStyleD3D);
			const RawCamera shadowMapCamera = RawCamera(lightToWsNoScaling.p, shadowViewMtx, shadowProjMtx);

			return ShadowMapBuildInfo(shadowMapCamera);
		}
		case light_point: {
			const vec3f camPosWs = lightWs.p;
			mat4f shadowProjMtx = mat4f::getPerspectiveFovRH(half_pi(), 1.f, 0.1f, range, kIsTexcoordStyleD3D);

			// Caution: [POINT_LIGHT_SHADOWMAP_TRIANGLE_WINING_FLIP]
			// When we render Cube maps depending on the rendering API we need to
			// modify the projection matrix so the final cube map is render correctly.
			// however these modification could change the determinant sign of the projection
			// matrix, resulting in flipping the triangle winding.
			if (kIsTexcoordStyleD3D) {
				// 1. A myterious X-axis flip, the same one is done for OpenGL.
				shadowProjMtx = mat4f::getScaling(-1.f, 1.f, 1.f) * shadowProjMtx;
			} else {
				// 1. A myterious x-axis flip, the same one is done for Direct3D.
				// 2. A Y-axis flip as texture space (0,0) for OpenGL is bottom left,
				// while we use it as top-left.
				// So 2 changes of the determinant sign => the sign remains the same for OpenGL.
				shadowProjMtx = mat4f::getScaling(-1.f, -1.f, 1.f) * shadowProjMtx;
			}

			// Compute the view matrix of each camera.
			mat4f perCamViewMtx[signedAxis_numElements];
			perCamViewMtx[axis_x_pos] =
			    mat4f::getLookAtRH(camPosWs, camPosWs + vec3f::getSignedAxis(axis_x_pos), vec3f::getSignedAxis(axis_y_pos));
			perCamViewMtx[axis_x_neg] =
			    mat4f::getLookAtRH(camPosWs, camPosWs + vec3f::getSignedAxis(axis_x_neg), vec3f::getSignedAxis(axis_y_pos));
			perCamViewMtx[axis_y_pos] =
			    mat4f::getLookAtRH(camPosWs, camPosWs + vec3f::getSignedAxis(axis_y_pos), vec3f::getSignedAxis(axis_z_neg));
			perCamViewMtx[axis_y_neg] =
			    mat4f::getLookAtRH(camPosWs, camPosWs + vec3f::getSignedAxis(axis_y_neg), vec3f::getSignedAxis(axis_z_pos));
			perCamViewMtx[axis_z_pos] =
			    mat4f::getLookAtRH(camPosWs, camPosWs + vec3f::getSignedAxis(axis_z_pos), vec3f::getSignedAxis(axis_y_pos));
			perCamViewMtx[axis_z_neg] =
			    mat4f::getLookAtRH(camPosWs, camPosWs + vec3f::getSignedAxis(axis_z_neg), vec3f::getSignedAxis(axis_y_pos));

			// TODO:
			// Mark cameras which do not intersect with the main camera frustum to not be rendered,
			// as their shadowmaps aren't going to get used.
			ShadowMapBuildInfo result;

			result.isPointLight = true;
			result.pointLightShadowMapCameras[axis_x_pos] = RawCamera(camPosWs, perCamViewMtx[axis_x_pos], shadowProjMtx);
			result.pointLightShadowMapCameras[axis_x_neg] = RawCamera(camPosWs, perCamViewMtx[axis_x_neg], shadowProjMtx);
			result.pointLightShadowMapCameras[axis_y_pos] = RawCamera(camPosWs, perCamViewMtx[axis_y_pos], shadowProjMtx);
			result.pointLightShadowMapCameras[axis_y_neg] = RawCamera(camPosWs, perCamViewMtx[axis_y_neg], shadowProjMtx);
			result.pointLightShadowMapCameras[axis_z_pos] = RawCamera(camPosWs, perCamViewMtx[axis_z_pos], shadowProjMtx);
			result.pointLightShadowMapCameras[axis_z_neg] = RawCamera(camPosWs, perCamViewMtx[axis_z_neg], shadowProjMtx);
			result.pointLightFarPlaneDistance = range;

			return result;
		} break;
		default: {
			sgeAssert(false && "Unimplemented light type!");
		}
	}

	return NullOptional();
}

//---------------------------------------------------------------------------
// ALight
//---------------------------------------------------------------------------
void ALight::create() {
	registerTrait(m_traitViewportIcon);
	m_traitViewportIcon.setTexture("assets/editor/textures/icons/obj/ALight.png", true);
}

AABox3f ALight::getBBoxOS() const {
	switch (m_lightDesc.type) {
		case light_directional: {
			return AABox3f();
		} break;
		case light_point: {
			AABox3f result;
			result.expand(vec3f(vec3f(m_lightDesc.range)));
			result.expand(vec3f(vec3f(-m_lightDesc.range)));
			return result;
		} break;
		case light_spot: {
			const float coneLength = maxOf(m_lightDesc.range, 2.f);
			const float coneRadius = tanf(m_lightDesc.spotLightAngle) * coneLength;
			AABox3f result;
			result.expand(vec3f(0.f));
			result.expand(vec3f(coneLength, coneRadius, coneRadius));
			result.expand(vec3f(coneLength, -coneRadius, -coneRadius));
			return result;
		};
		default:
			sgeAssert(false && "Not implemented for this light type");
			return AABox3f();
	}
}

void ALight::update(const GameUpdateSets& UNUSED(updateSets)) {

}


} // namespace sge
