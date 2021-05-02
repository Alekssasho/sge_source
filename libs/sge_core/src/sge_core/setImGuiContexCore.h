#pragma once

#include "imgui/imgui.h"
#include "sgecore_api.h"

SGE_CORE_API ImGuiContext* getImGuiContextCore();
SGE_CORE_API void setImGuiContextCore(ImGuiContext* g);