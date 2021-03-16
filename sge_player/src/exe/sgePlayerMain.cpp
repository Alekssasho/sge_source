#if 1
// This thing makes the driver to auto-choose the high-end GPU instead of the "integrated" one.
#ifdef _WIN32
extern "C" {
__declspec(dllexport) int NvOptimusEnablement = 0x00000001;
}
extern "C" {
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "GameMode.h"
#include "sge_core/AssetLibrary.h"
#include "sge_core/ICore.h"
#include "sge_core/QuickDraw.h"
#include "sge_core/SGEImGui.h"
#include "sge_core/application/application.h"
#include "sge_core/setImGuiContexCore.h"
#include "sge_engine/EngineGlobal.h"
#include "sge_engine/GamePlayerSettings.h"
#include "sge_engine/IPlugin.h"
#include "sge_engine/TypeRegister.h"
#include "sge_engine/setImGuiContextEngine.h"
#include "sge_utils/tiny/FileOpenDialog.h"
#include "sge_utils/utils/DLLHandler.h"
#include "sge_utils/utils/FileStream.h"
#include "sge_utils/utils/Path.h"
#include "sge_utils/utils/json.h"

#include <filesystem>
#include <thread>

#include "MiniDump.h"

using namespace sge;

int g_argc = 0;
char** g_argv = nullptr;

GamePlayerSettings g_playerSettings;

struct SGEGameWindow : public WindowBase {
	GamePlayerSettings playerSets;
	GameMode gameMode;

	Timer m_timer;
	std::string pluginName;
	DLLHandler m_dllHandler;
	IPlugin* m_pluginInst = nullptr;
	IGameDrawer* m_pGameDrawer = nullptr;

	vec2i cachedWindowSize = vec2i(0);

	void HandleEvent(const WindowEvent event, const void* const eventData) final {
		if (event == WE_Create) {
			OnCreate();
		}

		if (event == WE_Resize) {
			const WE_Resize_Data& resizeData = *static_cast<const WE_Resize_Data*>(eventData);
			cachedWindowSize.x = resizeData.width;
			cachedWindowSize.y = resizeData.height;

			SGEDevice* device = getCore()->getDevice();
			if (device) {
				const WE_Resize_Data& data = *(const WE_Resize_Data*)eventData;
				getCore()->getDevice()->resizeBackBuffer(data.width, data.height);
				SGEImGui::setViewport(Rect2s(short(data.width), short(data.height)));
			}
		}

		if (event == WE_Destroying) {
			SGEImGui::destroy();
			SGE_DEBUG_LOG("Destroy Called!");
		}
	}

	void OnCreate() {
		int const backBufferWidth = this->GetClientWidth();
		int const backBufferHeight = this->GetClientHeight();

		MainFrameTargetDesc mainTargetDesc;
		mainTargetDesc.width = backBufferWidth;
		mainTargetDesc.height = backBufferHeight;
		mainTargetDesc.numBuffers = 2;
		mainTargetDesc.vSync = true;
		mainTargetDesc.sampleDesc = SampleDesc(1);
#ifdef _WIN32
		mainTargetDesc.hWindow = GetNativeHandle();
		mainTargetDesc.bWindowed = true;
#endif

		// Obtain the backbuffer rendertarget
		// initialized the device and the immediate context
		SGEDevice* const device = SGEDevice::create(mainTargetDesc);

		SGEImGui::initialize(device->getContext(), device->getWindowFrameTarget(), &GetInputState(),
		                     device->getWindowFrameTarget()->getViewport());
		ImGui::SetCurrentContext(getImGuiContextCore());
		setImGuiContextEngine(getImGuiContextCore());

		getCore()->setup(device);
		getCore()->getAssetLib()->scanForAvailableAssets("assets");

		for (auto const& entry : std::filesystem::directory_iterator("./")) {
			if (std::filesystem::is_regular_file(entry) && entry.path().extension() == ".gll") {
				pluginName = entry.path().string();
			}
		}

		if (pluginName.empty()) {
			return;
		} else {
			loadPlugin();
		}

		m_pGameDrawer = m_pluginInst->allocateGameDrawer();

		typeLib().performRegistration();
		getEngineGlobal()->initialize();

		gameMode.create(m_pGameDrawer, g_playerSettings.initalLevel.c_str());
	}

	void loadPlugin() {
		// Notify that we are about to unload the plugin.
		getEngineGlobal()->notifyOnPluginPreUnload();

		// Unload the old plugin DLL and load the new one.
		m_dllHandler.load(pluginName.c_str());

		// Obain the new plugin interface.
		GetInpteropFnPtr interopGetter = reinterpret_cast<GetInpteropFnPtr>(m_dllHandler.getProcAdress("getInterop"));
		sgeAssert(interopGetter != nullptr && "The loaded game dll does not have a getInterop()!\n");
		if (interopGetter) {
			m_pluginInst = interopGetter();
		}

		getEngineGlobal()->changeActivePlugin(m_pluginInst);
	}

	void run() {
		m_timer.tick();
		getEngineGlobal()->update(m_timer.diff_seconds());


		float const bgColor[] = {0.f, 0.f, 0.f, 1.f};

		SGEImGui::newFrame();

		SGEContext* const sgecon = getCore()->getDevice()->getContext();
		sgecon->clearColor(getCore()->getDevice()->getWindowFrameTarget(), -1, bgColor);
		sgecon->clearDepth(getCore()->getDevice()->getWindowFrameTarget(), 1.f);

		if (m_pluginInst) {
			m_pluginInst->run();
		}

		gameMode.update(GetInputState());
		RenderDestination rdest = RenderDestination(sgecon, sgecon->getDevice()->getWindowFrameTarget());
		gameMode.draw(rdest);

		// Render the ImGui User Interface.
		SGEImGui::render();

		// Finally display everyting to the screen.
		getCore()->setLastFrameStatistics(getCore()->getDevice()->getFrameStatistics());
		getCore()->getDevice()->present();

		return;
	}
};

void main_loop() {
	sge::ApplicationHandler::get()->PollEvents();
	for (WindowBase* wnd : sge::ApplicationHandler::get()->getAllWindows()) {
		SGEGameWindow* gameWindow = dynamic_cast<SGEGameWindow*>(wnd);
		if (gameWindow) {
			gameWindow->run();
		}
	}
}

int sge_main(int argc, char** argv) {
	SGE_DEBUG_LOG("sge_main()\n");

	setlocale(LC_NUMERIC, "C");

	g_argc = argc;
	g_argv = argv;

	if (!g_playerSettings.loadFromJsonFile("appdata/game_project_settings.json")) {
		DialogYesNo("Error", "game_project_settings.json seems to be missing or invalid!");
		return 0;
	}

	vec2i windowSize(g_playerSettings.windowWidth, g_playerSettings.windowHeight);

	JsonParser jp;
	sge::ApplicationHandler::get()->NewWindow<SGEGameWindow>("SGEEngine Game Player", windowSize.x, windowSize.y, false,
	                                                         !g_playerSettings.windowIsResizable);

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, true);
#else
	while (sge::ApplicationHandler::get()->shouldStopRunning() == false) {
		main_loop();
	};
#endif

	return 0;
}


#ifdef __EMSCRIPTEN__
#include <SDL2/SDL.h>
#include <emscripten.h>
//#include <GLES3/gl3.h> // WebGL2 + GLES 3 emulation.
#else
#include <SDL.h>
#include <SDL_syswm.h>
#endif

// Caution:
// SDL2 might have a macro (depending on the target platform) for the main function!
int main(int argc, char* argv[]) {
	SGE_DEBUG_LOG("main()\n");
	sgeRegisterMiniDumpHandler();


	SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1");
#ifdef __EMSCRIPTEN__

	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

#else
	SDL_Init(SDL_INIT_EVERYTHING);
#ifdef SGE_RENDERER_D3D11
	// SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
#else if SGE_RENDERER_GL
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

	float ddpi = 0.f;
	SDL_GetDisplayDPI(0, &ddpi, nullptr, nullptr);

	// SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	// SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	// SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	// SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	// SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
#endif


	// const int numJoystics = SDL_NumJoysticks();
	// for (int iJoy = 0; iJoy < numJoystics; ++iJoy) {
	//	if (SDL_IsGameController(iJoy)) {
	//		[[maybe_unused]] SDL_GameController* const gameController = SDL_GameControllerOpen(iJoy);
	//	}
	//}
	return sge_main(argc, argv);
}
