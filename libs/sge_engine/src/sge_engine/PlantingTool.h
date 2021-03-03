#pragma once

#include <string>

#include "InspectorTool.h"
#include "sge_utils/math/Box.h"
#include "sge_utils/math/transform.h"
#include "sge_renderer/renderer/renderer.h"
#include "sge_utils/utils/vector_set.h"
#include "sge_engine/GameObject.h"

namespace sge {

struct Actor;

// Caution: WIP. Currenctly hardcoded to create static obstacles with some model attached.
struct SGE_ENGINE_API PlantingTool : public IInspectorTool {
	void setup(Actor* actorToPlant);
	void setup(const vector_set<ObjectId>& actorsToPlant, GameWorld& world);

	void onSetActive(GameInspector* const inspector) override final;
	void onUI(GameInspector* inspector) override final;
	InspectorToolResult updateTool(GameInspector* const inspector,
	                               bool isAllowedToTakeInput,
	                               const InputState& is,
	                               const GameDrawSets& drawSets) override final;
	void onCancel(GameInspector* inspector) override final;

	void drawOverlay(const GameDrawSets& drawSets) override final;

	std::string m_modelForStaticObstacle;
	vector_map<ObjectId, transf3d> actorsToPlantOriginalTrasnforms;
};

} // namespace sge
