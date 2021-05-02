#pragma once

#include "sge_core/AssetLibrary.h"

#include "sge_engine/Actor.h"
#include "sge_engine/RigidBodyEditorConfig.h"
#include "sge_engine/traits/TraitModel.h"
#include "sge_engine/traits/TraitRigidBody.h"

namespace sge {

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
struct SGE_ENGINE_API ADynamicObstacle : public Actor {
	ADynamicObstacle() = default;

	void create() final;
	void onPlayStateChanged(bool const isStartingToPlay) override;
	void onDuplocationComplete() final;
	void postUpdate(const GameUpdateSets& updateSets) final;

	AABox3f getBBoxOS() const final;

  public:
	TraitModel m_traitModel;
	TraitRigidBody m_traitRB;
	RigidBodyConfigurator m_rbConfig;
};



} // namespace sge
