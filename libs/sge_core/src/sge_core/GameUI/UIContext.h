#pragma once

#include "sge_core/sgecore_api.h"
#include "sge_renderer/renderer/renderer.h"
#include "sge_utils/math/vec2.h"
#include <memory>

namespace sge {

struct QuickDraw;
struct DebugFont;
struct InputState;

namespace gamegui {

	struct IWidget;

	struct UIDrawSets {
		void setup(RenderDestination rdest, QuickDraw* const quickDraw) {
			this->rdest = rdest;
			this->quickDraw = quickDraw;
		}

		bool isValid() const { return quickDraw != nullptr; }

		RenderDestination rdest;
		QuickDraw* quickDraw = nullptr;
	};

	///
	///
	///
	struct SGE_CORE_API UIContext {
		void create(const vec2i& canvasSize) { m_canvasSize = canvasSize; }

		void update(const InputState& is, const vec2i& canvasSize, const float dt);
		void draw(const UIDrawSets& drawSets);

		vec2i getCanvasSize() const { return m_canvasSize; }

		void addRootWidget(std::shared_ptr<IWidget> widget) { m_rootWidgets.emplace_back(widget); }

		void setCanvasSize(const vec2i& canvas) { m_canvasSize = canvas; }

		void setDefaultFont(DebugFont* f) { m_defaultFont = f; }

		DebugFont* getDefaultFont() { return m_defaultFont; }

		std::shared_ptr<IWidget> getGamepadTarget() { return m_gamepadTargetWeak.lock(); }

		void setGamepadTarget(std::weak_ptr<IWidget> wp) { m_gamepadTargetWeak = std::move(wp); }

	  private:
		bool m_isUsingGamepad = false;
		vec2i m_canvasSize = vec2i(0);
		std::vector<std::shared_ptr<IWidget>> m_rootWidgets;
		std::weak_ptr<IWidget> m_pressedWidgetWeak;
		std::weak_ptr<IWidget> m_gamepadTargetWeak;
		DebugFont* m_defaultFont = nullptr;
		float m_gamepadDirInputCooldown = 0.f;
	};
} // namespace gamegui


} // namespace sge
