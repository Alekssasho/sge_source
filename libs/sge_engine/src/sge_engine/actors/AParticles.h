#pragma once

#include "sge_engine/Actor.h"
#include "sge_engine/traits/TraitParticles.h"
#include "sge_engine/traits/TraitViewportIcon.h"
#include "sge_engine/traits/TraitCustomAE.h"

namespace sge {

DefineTypeIdExists(AParticles);
struct SGE_ENGINE_API AParticles : public Actor, public IActorCustomAttributeEditorTrait {
	AABox3f getBBoxOS() const final;
	void create() final;
	void postUpdate(const GameUpdateSets& u) final;

	virtual void doAttributeEditor(GameInspector* inspector) final;

  public:
	TraitParticles m_particles;
	TraitViewportIcon m_traitViewportIcon;

	int m_uiSelectedGroup = 0;
};

} // namespace sge
