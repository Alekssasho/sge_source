#pragma once

#include "sge_core/Animator.h"
#include "sge_core/shaders/modeldraw.h"
#include "sge_engine/Actor.h"
#include "sge_engine/AssetProperty.h"


namespace sge {

/// @brief TraitModel is a trait designed to be attached in an Actor.
/// It provides a simple way to assign a renderable 3D Model to the game object (both animated and static).
/// The trait is not automatically updateable, the user needs to manually call @update() method in their objects.
/// This is because not all game object need this complication of auto updating.
/// TODO: Better documentation, for now please take a look at how this class is used in the source.\
/// In short if your object is set to use only one model and this model cannot be changed form UI just use
/// TraitModel::setModel(pathToAsset, true) to initialize it once in Actor::create() method.
DefineTypeIdExists(TraitModel);
struct SGE_ENGINE_API TraitModel : public Trait {
	SGE_TraitDecl_Full(TraitModel);

	TraitModel()
	    : m_assetProperty(AssetType::Model) {}

	void setModel(const char* assetPath, bool updateNow) {
		m_assetProperty.setTargetAsset(assetPath);
		if (updateNow) {
			postUpdate();
		}
	}

	void setModel(std::shared_ptr<Asset>& asset) { m_assetProperty.setAsset(asset); }

	// Updates the working model.
	// Returns true if the model has been changed (no matter if it is valid or not).
	bool postUpdate() { return m_assetProperty.update(); }

	void clear() { m_assetProperty.clear(); }

	AssetProperty& getAssetProperty() { return m_assetProperty; }
	const AssetProperty& getAssetProperty() const { return m_assetProperty; }

	mat4f getAdditionalTransform() const { return m_additionalTransform; }
	void setAdditionalTransform(const mat4f& tr) { m_additionalTransform = tr; }

	mat4f getFullWorldMtx() const { return getActor()->getTransformMtx() * m_additionalTransform; }

	AABox3f getBBoxOS() const {
		const AssetModel* const assetModel = getAssetProperty().getAssetModel();
		if (assetModel && assetModel->staticEval.isInitialized()) {
			AABox3f bbox = assetModel->staticEval.aabox;
			return bbox;
		}

		return AABox3f(); // Return an empty box.
	}

	void setRenderable(bool v) { isRenderable = v; }
	bool getRenderable() const { return isRenderable; }
	void setNoLighting(bool v) { drawMods.forceNoLighting = v; }
	bool getNoLighting() const { return drawMods.forceNoLighting; }
	bool showOnlyInEditMode() const { return m_showOnlyInEditmode; }
	void setShowOnlyInEditMode(bool v) { m_showOnlyInEditmode = v; }

	void computeNodeToBoneIds();
	void computeSkeleton(vector_map<const Model::Node*, mat4f>& boneOverrides);

  private:
	void onModelChanged() {
		useSkeleton = false;
		rootSkeletonId = ObjectId();
		nodeToBoneId.clear();
	}

  public:
	struct MaterialOverride {
		std::string materialName;
		ObjectId materialObjId;
	};

	mat4f m_additionalTransform = mat4f::getIdentity();
	InstanceDrawMods drawMods;
	AssetProperty m_assetProperty;
	std::string animationName;
	float animationTime = 0.f;
	bool isRenderable = true;
	bool m_showOnlyInEditmode = false;

	bool useSkeleton = false;
	ObjectId rootSkeletonId;

	std::vector<MaterialOverride> m_materialOverrides;

	// Skeleton.
	std::unordered_map<const Model::Node*, ObjectId> nodeToBoneId;
};



} // namespace sge
