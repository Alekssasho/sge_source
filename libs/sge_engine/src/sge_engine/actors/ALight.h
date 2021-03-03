#pragma once

#include "sge_engine/traits/TraitCamera.h"
#include "sge_engine/Actor.h"
#include "sge_utils/utils/optional.h"

#include "sge_engine/traits/TraitViewportIcon.h"

namespace sge {

enum LightType {
	light_point,
	light_directional,
	light_spot,
};

struct ShadowMapBuildInfo {
	ShadowMapBuildInfo() = default;

	ShadowMapBuildInfo(RawCamera shadowMapCamera)
	    : shadowMapCamera(shadowMapCamera) {}

	bool isPointLight = false;
	RawCamera shadowMapCamera; // todo multiple camera for point lights.
	RawCamera pointLightShadowMapCameras[SignedAxis::signedAxis_numElements];
	float pointLightFarPlaneDistance = 0.f;
};

struct SGE_ENGINE_API LightDesc {
	/// True if the light should participate in the shading.
	bool isOn = true;
	/// The lights type.
	LightType type = light_point;
	/// The intensity of the light.
	float intensity = 1.f;
	/// The maximum illumination distance, the light will start fadeing into black as the shaded point gets away form the light source.
	float range = 10.f;
	/// For Spot light only.
	float spotLightAngle = deg2rad(30.f);
	vec3f color = vec3f(1.f);
	/// True if the light should cast shadows mapping.
	bool hasShadows = false;
	/// The resolution of the two sides of the shadow map texture.
	int shadowMapRes = 128;

	Optional<ShadowMapBuildInfo> buildShadowMapInfo(const transf3d& lightWs, const Frustum& mainCameraFrustumWs) const;
};

struct SGE_ENGINE_API ALight : public Actor {
	ALight() = default;

	void create() final;
	AABox3f getBBoxOS() const final;
	void update(const GameUpdateSets& updateSets) final;
	LightDesc getLightDesc() const { return m_lightDesc; }

  public:
	LightDesc m_lightDesc;
	TraitViewportIcon m_traitViewportIcon;
};


} // namespace sge
