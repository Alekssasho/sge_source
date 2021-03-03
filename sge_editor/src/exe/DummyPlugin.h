#pragma once

#include "sge_utils/sge_utils.h"
#include "sge_engine/IPlugin.h"

namespace sge {
struct DummyPlugin final : public IPlugin {
	WindowBase* mainWindow = nullptr;

	IGameDrawer* allocateGameDrawer() override;
	void onLoaded(const InteropPreviousState& UNUSED(prevState), ImGuiContext* UNUSED(imguiCtx), WindowBase* UNUSED(window), ICore* UNUSED(global)) final {}
	void onUnload(InteropPreviousState& UNUSED(outState)) final {}
	void run() final {}
	void handleEvent(WindowBase* UNUSED(window), const WindowEvent UNUSED(event), const void* const UNUSED(eventData)) final {}
};

} // namespace sge
