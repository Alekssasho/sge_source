#pragma once

#include "Camera.h"
#include "GameWorld.h"
#include "InspectorCmd.h"
#include "PlantingTool.h"
#include "SelectionTool.h"
#include "TransformTool.h"
#include "sge_engine_api.h"

namespace sge {

/// @brief GameInspector is a class that takes care of all the data that is not needed by the GameWorld when playing the game.
/// It also is the central hub for editor specific data about the game world, like the undo/redo commands, seleted objects, viewport tools
/// (to be removed from here) and more.
struct SGE_ENGINE_API GameInspector {
	GameInspector();

	GameInspector(const GameInspector&) = delete;
	GameInspector& operator=(const GameInspector&) = delete;

	GameWorld* getWorld() { return m_world; }
	const GameWorld* getWorld() const { return m_world; }

	bool isSteppingAllowed() const;

	// Commands.
	void redoCommand();
	void undoCommand();
	void appendCommand(InspectorCmd* const cmd, bool shouldApply);

	// Selection.
	bool hasSelection() const { return m_selection.size() != 0; }
	bool isSelected(ObjectId const id, bool* const outIsPrimary = nullptr) const;
	bool isPrimarySelected(ObjectId const id) const;
	const std::vector<SelectedItem>& getSelection() const { return m_selection; }
	ObjectId getPrimarySelection() const;
	ObjectId getSecondarySelectedActor() const;
	void getAllSelectedObjects(vector_set<ObjectId>& allActors);
	void select(ObjectId const id, bool const selectAsPrimary = false);
	void deselect(ObjectId const id);
	void toggleSelected(ObjectId const id);
	void deselectAll();
	void duplicateSelection(vector_set<ObjectId>* outNewObjects = nullptr);
	void deleteSelection(bool const deleteHierarchyUnderSelectedObjects);

	// Camera.
	void focusOnSelection();

	// Returns the camera that should be used for rendering the game.
	ICamera* getRenderCamera();

	// Tools
	void setTool(IInspectorTool* const tool) {
		if (m_toolInAction) {
			m_toolInAction->onCancel(this);
		}

		m_toolInAction = tool;

		if (m_toolInAction) {
			m_toolInAction->onSetActive(this);
		}
	};

  public: // TODO: Make this private
	void clear() {
		m_world = nullptr;
		editMode = editMode_actors;
		m_selection.clear();
		m_selectionChangeIdx = 0;
		m_toolInAction = nullptr;
		m_selectionTool = SelectionTool();
		m_transformTool = TransformTool();
		m_plantingTool = PlantingTool();

		m_physicsDebugDrawEnabled = false;

		m_useEditorCamera = true;
		m_editorCamera = EditorCamera();
		m_editorCamera.m_orbitCamera.yaw = -sgeHalfPi;
		m_editorCamera.m_orbitCamera.pitch = deg2rad(35.f);
		m_editorCamera.m_orbitCamera.radius = 20.f;
		m_editorCamera.m_projSets.fov = deg2rad(60.f);
		m_editorCamera.m_projSets.aspectRatio = 1.f; // Just some default to be overriden.
		m_editorCamera.m_projSets.near = 0.1f;
		m_editorCamera.m_projSets.far = 10000.f;

		m_disableAutoStepping = false;
		m_stepOnce = false;

		m_commandHistory.clear();
		m_lastExecutedCommandIdx = -1;
	}

	GameWorld* m_world = nullptr;
	EditMode editMode = editMode_actors;
	std::vector<SelectedItem> m_selection;
	// The number of times the selection has been chaged.
	int m_selectionChangeIdx = 0;

	// Tools.
	IInspectorTool* m_toolInAction = nullptr;
	SelectionTool m_selectionTool;
	TransformTool m_transformTool;
	PlantingTool m_plantingTool;

	bool m_physicsDebugDrawEnabled = false;

	// The editor camera and a boolen that specify if we should use it or we should the the game's camera.
	bool m_useEditorCamera = true;
	EditorCamera m_editorCamera;

	// Game stepping.
	bool m_disableAutoStepping = false;
	bool m_stepOnce = false;

	// Commands.
	std::vector<std::unique_ptr<InspectorCmd>> m_commandHistory;
	int m_lastExecutedCommandIdx = -1;
};

} // namespace sge
