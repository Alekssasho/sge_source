#pragma once

#include "sge_engine_api.h"
#include <string>

struct ImGuiContext;

namespace sge {

struct WindowBase;
enum WindowEvent : int;
struct ICore;
struct IGameDrawer;

struct SGE_ENGINE_API InteropPreviousState {
	bool isInitializationState = false;
	char** argv;
	int argc;
	bool valid = false;
	std::string level;
};

struct SGE_ENGINE_API IPlugin {
	virtual ~IPlugin() {}

	virtual IGameDrawer* allocateGameDrawer();

	virtual void onLoaded(const InteropPreviousState& prevState, ImGuiContext* imguiCtx, WindowBase* window, ICore* global) = 0;
	virtual void onUnload(InteropPreviousState& outState) = 0;
	virtual void run() = 0;
	virtual void handleEvent(WindowBase* window, const WindowEvent event, const void* const eventData) = 0;
};

typedef IPlugin* (*GetInpteropFnPtr)();

} // namespace sge
