#pragma once

#include "IImGuiWindow.h"
#include "imgui/imgui.h"
#include "sge_utils/utils/optional.h"
#include <string>

namespace sge {

struct GameInspector;

struct SGE_ENGINE_API PrefabWindow : public IImGuiWindow {
	PrefabWindow(std::string windowName, GameInspector& inspector)
	    : m_windowName(std::move(windowName))
	    , m_inspector(inspector) {}

	bool isClosed() override { return !m_isOpened; }
	void update(SGEContext* const sgecon, const InputState& is) override;
	const char* getWindowName() const override { return m_windowName.c_str(); }

  private:
	bool m_isOpened = true;
	std::string m_windowName;
	GameInspector& m_inspector;

	Optional<std::vector<std::string>> m_availablePrefabs;

	std::string createPrefabName;
};

} // namespace sge
