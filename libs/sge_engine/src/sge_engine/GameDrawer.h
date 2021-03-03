#pragma once

#include "Actor.h"
#include "sge_renderer/renderer/renderer.h"

namespace sge {

struct GameWorld;

struct QuickDraw;
struct ICamera;

struct ShadowMapBuildInfo;
struct IGameDrawer;

enum DrawReason : int {
	drawReason_editing,
	drawReason_selectionTool,
	drawReason_wireframePrimary,
	drawReason_wireframe,

	drawReason_gameplay,
	drawReason_gameplayShadow,
};

// Returns true if we are rendering a to visualize selection or some other helper usually as an overaly on top of the existing scene.
inline bool drawReason_IsWireframe(DrawReason const reason) {
	return reason == drawReason_wireframePrimary || reason == drawReason_wireframe;
}

// Returns true if the drawing pupouse is to fully visualize the game with its edditing tools and helpers like
// Light wireframes, paths lines, locators and so on.
inline bool drawReason_IsSelection(DrawReason const reason) {
	return reason == drawReason_selectionTool;
}

inline bool drawReason_IsEditView(DrawReason const reason) {
	return reason == drawReason_editing || drawReason_IsWireframe(reason);
}

inline bool drawReason_IsEditOrSelection(DrawReason const reason) {
	return reason == drawReason_selectionTool || drawReason_IsEditView(reason) || drawReason_IsWireframe(reason);
}

// Returns true if the rending is becuase we are displaying what the user should see.
inline bool drawReason_IsGameplay(DrawReason const reason) {
	return reason == drawReason_gameplay || reason == drawReason_gameplayShadow;
}

// Returns true if we are rendering the actual game or something in editmode.
inline bool drawReason_IsGameOrEdit(DrawReason const reason) {
	return drawReason_IsGameplay(reason) || drawReason_IsEditOrSelection(reason);
}

inline bool drawReason_IsGameOrEditNoShadowPass(DrawReason const reason) {
	return (drawReason_IsGameplay(reason) || drawReason_IsEditOrSelection(reason)) && reason != drawReason_gameplayShadow;
}

//--------------------------------------------------------------------
// GameDrawSets
//-------------------------------------------------------------------
struct GameDrawSets {
	void setup(SGEContext* const sgecon,
	           FrameTarget* const colorTarget,
	           QuickDraw* const debugDraw,
	           ICamera* const drawCamera,
	           ICamera* const gameCamera,
	           IGameDrawer* gameDrawer) {
		rdest = RenderDestination(sgecon, colorTarget);
		this->quickDraw = debugDraw;
		this->drawCamera = drawCamera;
		this->gameCamera = gameCamera;
		this->gameDrawer = gameDrawer;
	}

	bool isValid() const { return quickDraw != nullptr && drawCamera != nullptr; }

	RenderDestination rdest;
	QuickDraw* quickDraw = nullptr;
	ICamera* drawCamera = nullptr; // The camera that is going to be used in this rendering.
	ICamera* gameCamera = nullptr; // The camera that the player is going to be using.
	IGameDrawer* gameDrawer = nullptr;
	ShadowMapBuildInfo* shadowMapBuildInfo = nullptr; // The build info of the shadow map we are currenty rendering (if we do).
};

//--------------------------------------------------------------------
// IGameDrawer
//-------------------------------------------------------------------
struct SGE_ENGINE_API IGameDrawer {
	IGameDrawer() = default;
	virtual ~IGameDrawer() = default;

	virtual void initialize(GameWorld* const world);

	virtual void prepareForNewFrame() = 0;
	/// TODO: should we embed this in @prepareForNewFrame()
	virtual void updateShadowMaps(const GameDrawSets& drawSets) = 0;

	virtual void drawWorld(const GameDrawSets& drawSets, const DrawReason drawReason);
	virtual void drawActor(
	    const GameDrawSets& drawSets, EditMode const editMode, Actor* actor, int const itemIndex, DrawReason const drawReason) = 0;

	bool drawItem(const GameDrawSets& drawSets, const SelectedItem& item, bool const selectionMode);

	GameWorld* getWorld() { return m_world; }
	const GameWorld* getWorld() const { return m_world; }

  private:
	GameWorld* m_world = nullptr;
};

} // namespace sge
