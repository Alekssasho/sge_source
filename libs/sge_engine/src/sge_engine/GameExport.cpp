#include "GameExport.h"
#include "sge_utils/utils/Path.h"
#include <filesystem>

namespace sge {
void exportGame(const std::string& exportDir) {
	if (exportDir.empty()) {
		return;
	}

#define SGE_TRY_CATCH(code) \
	try {                   \
		code;               \
	} catch (...) {         \
	};

	const auto copyDirRecOverwrite = std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive;
	const auto copyOverwrite = std::filesystem::copy_options::overwrite_existing;

	SGE_TRY_CATCH(std::filesystem::copy("appdata", exportDir + "/appData", copyDirRecOverwrite));
	SGE_TRY_CATCH(std::filesystem::copy("assets", exportDir + "/assets", copyDirRecOverwrite));
	SGE_TRY_CATCH(std::filesystem::copy("core_shaders", exportDir + "/core_shaders", copyOverwrite));
	SGE_TRY_CATCH(std::filesystem::copy("SDL2d.dll", exportDir + "/SDL2d.dll", copyOverwrite));
	SGE_TRY_CATCH(std::filesystem::copy("SDL2.dll", exportDir + "/SDL2.dll", copyOverwrite));
	SGE_TRY_CATCH(std::filesystem::copy("sge_core.dll", exportDir + "/sge_core.dll", copyOverwrite));
	SGE_TRY_CATCH(std::filesystem::copy("sge_engine.dll", exportDir + "/sge_engine.dll", copyOverwrite));
	SGE_TRY_CATCH(std::filesystem::copy("sge_player.exe", exportDir + "/sge_player.exe", copyOverwrite));

	std::string pluginName;
	for (auto const& entry : std::filesystem::directory_iterator("./")) {
		if (std::filesystem::is_regular_file(entry) && entry.path().extension() == ".gll") {
			pluginName = entry.path().string();
		}
	}

	if (pluginName.empty() == false) {
		SGE_TRY_CATCH(std::filesystem::copy(pluginName, exportDir + pluginName));
	}
}

} // namespace sge
