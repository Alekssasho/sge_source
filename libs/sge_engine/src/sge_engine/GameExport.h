#pragma once

#include "sge_engine/sge_engine_api.h"
#include <string>

namespace sge {

/// @brief Exports the current state of the whole project as a standalone application
/// in the specified directory.
/// @param exportDir the path to the directory where the games is going to be exported.
SGE_ENGINE_API void exportGame(const std::string& exportDir);
} // namespace sge
