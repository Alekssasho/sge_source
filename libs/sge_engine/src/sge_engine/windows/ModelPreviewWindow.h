#pragma once


#include "IImGuiWindow.h"
#include "imgui/imgui.h"
#include "sge_engine/GameDrawer.h"


#include "sge_core/model/EvaluatedModel.h"
#include "sge_core/shaders/modeldraw.h"
#include "sge_renderer/renderer/renderer.h"
#include "sge_utils/tiny/orbit_camera.h"

namespace sge {

struct SGE_ENGINE_API ModelPreviewWidget {
	struct MomentDataUI {
		bool isEnabled = true;
		std::shared_ptr<Asset> modelAsset;
		int animationMagicIndex = 0; // 0 is static moment, eveything else is the animation index + 1
		EvalMomentSets moment;       // The actual moment that is going to be used.
	};

	bool m_autoPlay = true;
	orbit_camera camera;
	GpuHandle<FrameTarget> m_frameTarget;

	void doWidget(SGEContext* const sgecon, const InputState& is, EvaluatedModel& m_eval, Optional<vec2f> widgetSize = NullOptional());
};


struct ModelPreviewWindow : public IImGuiWindow {
	struct MomentDataUI {
		bool isEnabled = true;
		std::shared_ptr<Asset> modelAsset;
		int animationMagicIndex = 0; // 0 is static moment, eveything else is the animation index + 1
		EvalMomentSets moment;       // The actual moment that is going to be used.
	};

  public:
	ModelPreviewWindow(std::string windowName, bool createAsChild = false)
	    : m_windowName(std::move(windowName))
	    , m_createAsChild(createAsChild) {}

	bool isClosed() override { return !m_isOpened; }
	void update(SGEContext* const sgecon, const InputState& is) override;
	const char* getWindowName() const override { return m_windowName.c_str(); }

	std::shared_ptr<Asset>& getModel() { return m_model; }

  private:
	void doMomentUI(MomentDataUI& moment);

  private:
	std::string m_windowName;
	bool m_createAsChild = false;
	bool m_isOpened;

	bool m_autoPlay = true;
	std::shared_ptr<Asset> m_model;
	std::vector<MomentDataUI> m_momentsUI;
	std::vector<EvalMomentSets> m_moments; // A structure used to avoid unnecesarry allocations.
	GpuHandle<FrameTarget> m_frameTarget;

	orbit_camera camera;

	EvaluatedModel m_eval;
};


} // namespace sge
