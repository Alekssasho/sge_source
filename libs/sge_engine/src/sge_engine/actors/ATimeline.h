#pragma once

#include "sge_engine/Actor.h"
#include "sge_engine/GameInspector.h"
#include "sge_engine/InspectorCmd.h"
#include "sge_engine/traits/TraitCustomAE.h"
#include "sge_engine/traits/TraitModel.h"
#include "sge_engine/traits/TraitPath.h"
#include "sge_engine/traits/TraitViewportIcon.h"
#include "sge_utils/utils/vector_map.h"

namespace sge {

//--------------------------------------------------------
// ATimeline
//--------------------------------------------------------
struct ATimeline : public Actor, public IActorCustomAttributeEditorTrait {
	AABox3f getBBoxOS() const override;

	void create() override;
	void postUpdate(const GameUpdateSets& updateSets) override;

	// IActorCustomAttributeEditorTrait
	void doAttributeEditor(GameInspector* inspector) override;

	float getRawAnimationLength() const {
		const float res = keyFrames.size() == 0 ? 0.f : keyFrames.getAllKeys().back();
		return res;
	}

  public:
	TraitViewportIcon ttViewportIcon;

	bool isInEditMode = false;
	bool doesEditModeNeedsUpdate = false;

	bool m_isEnabled = false;
	PathLengthFollow::Settings playbackSettings;
	PathLengthFollow::State followState;
	PathLengthFollow::State editModeFollowState;
	bool moveObjectsOnTop = true;

	ObjectId targetActorId; // The object that we are going to move.
	int framesPerSecond = 30;
	int frameCount = 30;
	vector_map<int, transf3d> keyFrames;
};


} // namespace sge
