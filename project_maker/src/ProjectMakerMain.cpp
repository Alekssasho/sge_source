#include "IconsForkAwesome/IconsForkAwesome.h"
#include "cmakeCode.h"
#include "sge_core/AssetLibrary.h"
#include "sge_core/ICore.h"
#include "sge_core/QuickDraw.h"
#include "sge_core/SGEImGui.h"
#include "sge_core/application/application.h"
#include "sge_core/setImGuiContexCore.h"
#include "sge_utils/tiny/FileOpenDialog.h"
#include "sge_utils/utils/DLLHandler.h"
#include "sge_utils/utils/FileStream.h"
#include "sge_utils/utils/Path.h"
#include "sge_utils/utils/json.h"
#include <filesystem>
#include <fstream>
#include <regex>
#include <thread>

using namespace sge;

int g_argc = 0;
char** g_argv = nullptr;

struct SGEGameWindow : public WindowBase {
	Timer m_timer;

	void HandleEvent(const WindowEvent event, const void* const eventData) final {
		if (event == WE_Create) {
			OnCreate();
		}

		if (event == WE_Resize) {
			SGEDevice* device = getCore()->getDevice();

			if (device) {
				const WE_Resize_Data& data = *(const WE_Resize_Data*)eventData;
				getCore()->getDevice()->resizeBackBuffer(data.width, data.height);
				SGEImGui::setViewport(Rect2s(short(data.width), short(data.height)));
				getCore()->getQuickDraw().changeRenderDest(device->getContext(), device->getWindowFrameTarget(),
				                                           device->getWindowFrameTarget()->getViewport());
			}
		}

		if (event == WE_Destroying) {
			SGEImGui::destroy();
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
		ImGui::GetIO().IniFilename = nullptr;

		getCore()->setup(device);

		try {
			sgeEngineInstallDir = std::filesystem::current_path().parent_path().parent_path().string();
		} catch (...) {
			sgeEngineInstallDir = std::string();
		}
	}

	void run() {
		// Keep these the same as cmake generator for simplicity.
		const char* cmakeConfigOptions[] = {
#ifndef WIN32
		    "Unix Makefiles"
#else
		    "Visual Studio 16 2019",
		    "Visual Studio 15 2017",
#endif
		};

		m_timer.tick();

		float const bgColor[] = {0.f, 0.f, 0.f, 1.f};

		SGEImGui::newFrame();

		SGEContext* const sgecon = getCore()->getDevice()->getContext();
		sgecon->clearColor(getCore()->getDevice()->getWindowFrameTarget(), -1, bgColor);
		sgecon->clearDepth(getCore()->getDevice()->getWindowFrameTarget(), 1.f);

		ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
		ImGui::SetNextWindowSize(ImVec2(float(this->GetClientWidth()), float(this->GetClientHeight())));

		int windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin("Create Project Window", nullptr, windowFlags)) {
			// Prompt for Project name
			ImGuiEx::Label("Project Name:");
			ImGuiEx::InputText("##ProjectName", projectName, ImGuiInputTextFlags_CharsNoBlank, nullptr, nullptr, true);
			ImGui::NewLine();
			ImGui::NewLine();

			// Prompt for the directory that will contain the project directory.
			ImGui::Text("Pick directory that will host the Project: ");
			ImGuiEx::InputText("##DirInput", projectDirParent);
			ImGui::SameLine();
			if (ImGui::Button(ICON_FK_FOLDER_OPEN "## Pick Dir Button")) {
				projectDirParent = FolderOpenDialog("Pick a folder that will host the project folder:", "");
			}
			ImGui::NewLine();
			ImGui::NewLine();

			// Show a preview of the output dir.
			if (!projectDirParent.empty() && !projectName.empty()) {
				std::string previewProjectLocation = projectDirParent + "/" + projectName + "/";
				previewProjectLocation = canonizePathRespectOS(previewProjectLocation.c_str());
				ImGui::Text("The project location will be: %s", previewProjectLocation.c_str());
			} else {
				ImGui::NewLine();
			}

			// Cmake config settings.
			ImGui::NewLine();
			if (ImGui::CollapsingHeader("Cmake Configure Setting")) {
				ImGuiEx::Label("Auto Configure CMake ");
				ImGui::Checkbox("##Cmakeautoconfig", &autoConfigureCmake);

				if (ImGui::BeginCombo("Project Type", cmakeConfigOptions[cmakeConfigOptionsChoice])) {
					for (int iOpt = 0; iOpt < SGE_ARRSZ(cmakeConfigOptions); ++iOpt) {
						if (ImGui::Selectable(cmakeConfigOptions[iOpt])) {
							cmakeConfigOptionsChoice = iOpt;
						}
					}

					ImGui::EndCombo();
				}
			}

			if (showGameDirExistErrorMsg) {
				ImGui::TextColored(ImColor(1.f, 1.f, 0.f), "It appears that the directory is already in use! Pick another one!");
			} else {
				ImGui::NewLine();
			}

			// Create Button.
			if (ImGui::Button(ICON_FK_CHECK " Create!")) {
				std::string cmakeCode = cmakeGameProjectCode;
				cmakeCode = std::regex_replace(cmakeCode, std::regex("TemplateGame"), projectName);

				std::string projectRootDir = projectDirParent + "/" + projectName + "/";

				showGameDirExistErrorMsg = std::filesystem::exists(projectRootDir);
				if (showGameDirExistErrorMsg == false) {
					std::filesystem::create_directories(projectRootDir);

					std::string projectCmakeCodeDir = projectRootDir + "/game/";
					std::filesystem::create_directories(projectCmakeCodeDir);

					std::string projectCmakeCodeSrcDir = projectRootDir + "/game/src/";
					std::filesystem::create_directories(projectCmakeCodeSrcDir);

					{
						std::ofstream cmakeFileOfstream(projectCmakeCodeSrcDir + "PluginMain.cpp");
						cmakeFileOfstream << pluginMainCode;
						cmakeFileOfstream.close();
					}

					std::string projectCmakeBinDir = projectRootDir + "game_cmakebin/";
					std::filesystem::create_directories(projectCmakeBinDir);

					{
						std::ofstream cmakeFileOfstream(projectCmakeCodeDir + "CMakeLists.txt");
						cmakeFileOfstream << cmakeCode;
						cmakeFileOfstream.close();
					}

					if (autoConfigureCmake) {
						std::string cmd = "cmake -B\"" + projectCmakeBinDir + "\" -S\"" + projectCmakeCodeDir + "\" -DSGE_ENGINE_DIR=\"" +
						                  sgeEngineInstallDir + "\"";
						cmd += " -DCMAKE_INSTALL_PREFIX=\"" + projectCmakeCodeDir + "/game_bin/" + "\"";

						if (kIsTexcoordStyleD3D) {
							cmd += " -DSGE_REND_API=Direct3D11";
						} else {
							cmd += " -DSGE_REND_API=OpenGL";
						}

						cmd += " -G\"" + std::string(cmakeConfigOptions[cmakeConfigOptionsChoice]) + "\"";

						system(cmd.c_str());
#if WIN32
						std::string cmdOpenVS = "\"" + projectCmakeBinDir + projectName + ".sln\"";
						system(cmdOpenVS.c_str());

#endif
					}
				}
			}
		}
		ImGui::End();

		// Render the ImGui User Interface.
		SGEImGui::render();

		// Finally display everyting to the screen.
		getCore()->setLastFrameStatistics(getCore()->getDevice()->getFrameStatistics());
		getCore()->getDevice()->present();

		return;
	}

	int cmakeConfigOptionsChoice = 0;
	bool autoConfigureCmake = true;
	std::string projectName = "CoolGame";
	std::string projectDirParent = std::filesystem::current_path().string();
	std::string cmakeConfigGameBinDir = "${CMAKE_SOURCE_DIR}/game_bin";
	bool showGameDirExistErrorMsg = false;
	std::string sgeEngineInstallDir;
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


	sge::ApplicationHandler::get()->NewWindow<SGEGameWindow>("SGE Editor", 800, 600, false, true);

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
