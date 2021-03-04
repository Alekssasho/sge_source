#pragma once

#include "sge_engine/sge_engine_api.h"
#include <string>

namespace sge {

/// @brief Contains settings about how sge_player should launch the game.
/// As a reminder sge_player is how the final game is launched without the editor.
/// This is how the final software is suppoused to be distributed.
struct SGE_ENGINE_API GamePlayerSettings {
	GamePlayerSettings() = default;
	~GamePlayerSettings() = default;

	/// @brief Loads the GamePlayerSettings from the specified json file.
	/// @param outSettings contains the loaded settings.
	/// @param filename is the path to the json file containing the settings.
	/// @return returns true if the data was loaded successfuly and if it appears to be valid.
	bool loadFromJsonFile(const char* filename);

	/// @brief Saves the current settings to the specified json file.
	/// @param filename is the path to the json file to be saved.
	/// @return true if the saving has successful.
	bool saveToJsonFile(const char* filename) const;

  public:
	/// @brief The inital window width of the game window.
	int windowWidth = 640;

	/// @brief The inital window height of the game window.
	int windowHeight = 480;

	/// @brief True if the window of the game should be resizable.
	bool windowIsResizable = true;

	/// @brief The 1st level to be opened when the game runs in the sge_player (basically in game mode).
	std::string initalLevel;
};

} // namespace sge
