#include "ProjectSettingsWindow.h"
#include "IconsForkAwesome/IconsForkAwesome.h"
#include "sge_core/SGEImGui.h"
#include "sge_engine/EngineGlobal.h"
#include "sge_utils/tiny/FileOpenDialog.h"
#include <filesystem>

namespace sge {
ProjectSettingsWindow::ProjectSettingsWindow(std::string windowName)
    : m_windowName(std::move(windowName)) {
	m_gamePlayerSetting.loadFromJsonFile("appdata/game_project_settings.json");
}

void ProjectSettingsWindow::update(SGEContext* const UNUSED(sgecon), const InputState& UNUSED(is)) {
	if (isClosed()) {
		return;
	}

	if (ImGui::Begin(m_windowName.c_str(), &m_isOpened)) {
		ImGuiEx::Label("Window Width");
		ImGui::DragInt("##Window Width", &m_gamePlayerSetting.windowWidth, 1.f, 0, 100000);

		ImGuiEx::Label("Window Heigth");
		ImGui::DragInt("##Window Heigth", &m_gamePlayerSetting.windowHeight, 1.f, 0, 100000);

		ImGuiEx::Label("Is Window Resizable");
		ImGui::Checkbox("##Is Resizable", &m_gamePlayerSetting.windowIsResizable);

		ImGuiEx::Label("Initial Level");
		if (ImGui::Button(ICON_FK_FOLDER_OPEN)) {
			std::string pickedLevel =
			    FileOpenDialog("Select a the Initial level when the game is launched...", true, "*.lvl\0*.lvl\0", "./assets/levels");
			if (pickedLevel.empty() == false) {
				m_gamePlayerSetting.initalLevel = pickedLevel;
			}
		}

		ImGui::SameLine();
		ImGuiEx::InputText("##Inital Level input text", m_gamePlayerSetting.initalLevel);

		if (ImGui::Button(ICON_FK_FLOPPY_O " Save")) {
			std::filesystem::create_directories("appdata");
			m_gamePlayerSetting.saveToJsonFile("appdata/game_project_settings.json");
		}
	}
	ImGui::End();
}
} // namespace sge
