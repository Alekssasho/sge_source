#pragma once

#include "sge_core/Animator.h"
#include "sge_core/shaders/modeldraw.h"
#include "sge_engine/Actor.h"
#include "sge_engine/AssetProperty.h"


namespace sge {

/// @brief TraitModel is a trait designed to be attached in an Actor.
/// It provides a simple way to assign a renderable 3D Model to the game object (both animated and static).
/// The trait is not automatically updateable, the user needs to manually call @postUpdate() method in their objects.
/// This is because not all game object need this complication of auto updating.
///
/// For Example:
///
///    Lets say that you have a actor that is some collectable, a coin from Super Mario.
///    That coin 3D model is never going to change, you know that the game object is only going to use
///    that one specfic 3D model that you can set in @Actor::create method with @TraitModel::setModel() and forget about it.
///    You do not need any upadates on it, nor you want to be able to change the 3D model from the Property Editor Window.
///    In this situation you just add the trait, set the model and you are done.
///
///    The situation where the 3D model might change is for example with some Decore.
///    Lets say that your 3D artist has prepared a grass and bush models that you want to scatter around the level.
///    It would be thedious to have a separate game object for each 3D model.
///    Instead you might do what the @AStaticObstacle does, have the model be changed via Propery Editor Window.
///    In that way you have I generic actor type that could be configured to your desiers.
///    In order for the game object to take into account the change you need in your Actor::postUpdate to
///    to update the trait, see if the model has been changed and maybe update the rigid body for that actor.
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

	void setModel(std::shared_ptr<Asset>& asset, bool updateNow) {
		m_assetProperty.setAsset(asset);
		if (updateNow) {
			postUpdate();
		}
	}

	/// Not called automatically see the class comment above.
	/// Updates the working model.
	/// Returns true if the model has been changed (no matter if it is valid or not).
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
