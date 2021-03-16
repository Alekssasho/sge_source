#pragma once

#include "sge_engine_api.h"
#include <string>

struct ImGuiContext;

namespace sge {

struct ICore;
struct IGameDrawer;

struct SGE_ENGINE_API IPlugin {
	virtual ~IPlugin() {}

	virtual IGameDrawer* allocateGameDrawer();

	virtual void onLoaded(ImGuiContext* imguiCtx, ICore* global) = 0;
	virtual void onUnload() = 0;
	virtual void run() = 0;
};

typedef IPlugin* (*GetInpteropFnPtr)();

} // namespace sge
