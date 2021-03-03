#pragma once

#include "sge_engine/Actor.h"
#include "sge_core/Geometry.h"
#include "sge_utils/math/mat4.h"

namespace sge {

DefineTypeIdExists(TraitRenderableGeom);
/// Represents a custom made geometry that is going to be rendered with
/// the default shaders. (TODO: custom shaders and materials).
struct SGE_ENGINE_API TraitRenderableGeom : public Trait {
	SGE_TraitDecl_Full(TraitRenderableGeom);

	struct Element {
		const Geometry* pGeom = nullptr;
		const Material* pMtl = nullptr;
		mat4f tform = mat4f::getIdentity();
		/// If true tform should be used as if it specified the world space otherwise it is in object space of the owning actor.
		bool isTformInWorldSpace = false;
		bool isRenderable = true;
	};

	std::vector<Element> geoms;
};

} // namespace sge
