#include "GameMode.h"
#include "sge_core/ICore.h"
#include "sge_core/QuickDraw.h"

namespace sge {
void GameMode::create(IGameDrawer* gameDrawer, const char* openingLevelPath) {
	if(openingLevelPath == nullptr) {
		return;
	}

	m_sceneInstance.loadWorldFromFile(openingLevelPath, false);
	m_sceneInstance.getInspector().m_disableAutoStepping = false;
	m_sceneInstance.getInspector().m_useEditorCamera = false;
	m_gameDrawer = gameDrawer;
	m_gameDrawer->initialize(&m_sceneInstance.getWorld());
}

void GameMode::update(const InputState& is) {
	m_timer.tick();
	// m_sceneInstance.getInspector().m_disableAutoStepping = false;
	m_sceneInstance.update(m_timer.diff_seconds(), is);
}

void GameMode::draw(const RenderDestination& rdest) {
	GameWorld* const world = m_gameDrawer->getWorld();
	GameInspector* const inspector = world->getInspector();

	// Update the view aspect ratio.
	world->userProjectionSettings.canvasSize = vec2i(rdest.viewport.width, rdest.viewport.height);
	world->userProjectionSettings.aspectRatio = (float)rdest.viewport.width / (float)rdest.viewport.height;

	// Intilize the game draw settings.
	ICamera* const gameCamera = inspector->getRenderCamera();

	GameDrawSets drawSets;
	drawSets.rdest = rdest;
	drawSets.quickDraw = &getCore()->getQuickDraw();
	drawSets.drawCamera = gameCamera;
	drawSets.gameCamera = gameCamera;
	drawSets.gameDrawer = m_gameDrawer;

	drawSets.quickDraw->changeRenderDest(drawSets.rdest);

	// Draw.
	m_gameDrawer->prepareForNewFrame();
	m_gameDrawer->updateShadowMaps(drawSets);

	drawSets.rdest = rdest;
	drawSets.quickDraw = &getCore()->getQuickDraw();
	drawSets.drawCamera = gameCamera;
	drawSets.gameCamera = gameCamera;

	m_gameDrawer->drawWorld(drawSets, inspector->m_useEditorCamera ? drawReason_editing : drawReason_gameplay);
}

} // namespace sge
