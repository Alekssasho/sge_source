#include "sge_engine/IPlugin.h"
#include "sge_core/shaders/modeldraw.h"
#include "sge_engine/DefaultGameDrawer.h"

namespace sge
{
	struct PluginGame final : public IPlugin {
		virtual IGameDrawer* allocateGameDrawer() {
			return new DefaultGameDrawer();
		}

		virtual void onLoaded(const InteropPreviousState& prevState, ImGuiContext* imguiCtx, WindowBase* window, ICore* global) {}
		virtual void onUnload(InteropPreviousState& outState) {  }
		virtual void run() {}
		virtual void handleEvent(WindowBase* window, const WindowEvent event, const void* const eventData) {}
	};
}

extern "C" {
#ifdef WIN32
__declspec(dllexport) sge::IPlugin* getInterop() {
	return new sge::PluginGame();
}
#else
__attribute__((visibility("default"))) sge::IPlugin* getInterop() {
	return new sge::PluginGame();
}
#endif
}