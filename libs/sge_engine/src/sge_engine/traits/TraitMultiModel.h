#pragma once

#include "sge_core/ICore.h"
#include "sge_core/shaders/modeldraw.h"

#include "sge_engine/Actor.h"
#include "sge_engine/AssetProperty.h"

namespace sge {

DefineTypeIdExists(TraitMultiModel);
struct SGE_ENGINE_API TraitMultiModel : public Trait {
	struct Element {
		// TODO: this should end up being just multiple TraitModels, not it's own separate implementation.
		bool isAdditionalTransformInWorldSpace = false;
		mat4f additionalTransform = mat4f::getIdentity();
		AssetProperty assetProperty = AssetProperty(AssetType::Model);
		bool isRenderable = true;
		bool forceNoLighting = false;
	};

  public:
	SGE_TraitDecl_Full(TraitMultiModel);

	AABox3f getBBoxOS() const;

  public:
	std::vector<Element> models;
};

} // namespace sge
