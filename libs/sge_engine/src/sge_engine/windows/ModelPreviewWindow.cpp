#include "ModelPreviewWindow.h"
#include "sge_core/AssetLibrary.h"
#include "sge_core/QuickDraw.h"
#include "sge_core/SGEImGui.h"
#include "sge_core/application/input.h"
#include "sge_renderer/renderer/renderer.h"
#include "sge_utils/tiny/FileOpenDialog.h"
#include <imgui/imgui.h>

#include "sge_core/ICore.h"

namespace sge {

static void promptForModel(std::shared_ptr<Asset>& asset) {
	AssetLibrary* const assetLib = getCore()->getAssetLib();

	const std::string filename = FileOpenDialog("Pick a model", true, "*.mdl\0*.mdl\0");
	if (!filename.empty()) {
		asset = assetLib->getAsset(AssetType::Model, filename.c_str(), true);
	}
}

void ModelPreviewWidget::doWidget(SGEContext* const sgecon, const InputState& is, EvaluatedModel& m_eval, Optional<vec2f> widgetSize) {
	if (m_frameTarget.IsResourceValid() == false) {
		m_frameTarget = sgecon->getDevice()->requestResource<FrameTarget>();
		m_frameTarget->create2D(64, 64);
	}

	RenderDestination rdest(sgecon, m_frameTarget);


	const ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
	vec2f canvas_size = widgetSize.isValid() ? widgetSize.get() : fromImGui(ImGui::GetContentRegionAvail());

	if (canvas_size.x < 0.f)
		canvas_size.x = ImGui::GetContentRegionAvail().x;
	if (canvas_size.y < 0.f)
		canvas_size.y = ImGui::GetContentRegionAvail().y;

	if (canvas_size.y < 64)
		canvas_size.y = 64;

	vec2f texsize = canvas_size;

	if (texsize.x < 64)
		texsize.x = 64;
	if (texsize.y < 64)
		texsize.y = 64;

	if (m_frameTarget->getWidth() != texsize.x || m_frameTarget->getHeight() != texsize.y) {
		m_frameTarget->create2D((int)texsize.x, (int)texsize.y);
	}

	float ratio = (float)m_frameTarget->getWidth() / (float)m_frameTarget->getHeight();

	mat4f lookAt = camera.GetViewMatrix();
	mat4f proj = mat4f::getPerspectiveFovRH(deg2rad(45.f), ratio, 0.1f, 10000.f,
	                                        kIsTexcoordStyleD3D); // The Y flip for OpenGL is done by the modelviewer.

	sgecon->clearColor(m_frameTarget, 0, vec4f(0.f).data);
	sgecon->clearDepth(m_frameTarget, 1.f);

	QuickDraw& debugDraw = getCore()->getQuickDraw();


	debugDraw.drawWired_Clear();
	debugDraw.drawWiredAdd_Grid(vec3f(0), vec3f::getAxis(0), vec3f::getAxis(2), 5, 5, 0xFF333733);
	debugDraw.drawWired_Execute(rdest, proj * lookAt, nullptr);

	GeneralDrawMod mods;
	InstanceDrawMods imods;
	imods.forceNoLighting = true;

	getCore()->getModelDraw().draw(rdest, camera.eyePosition(), -camera.orbitPoint.normalized0(), proj * lookAt, mat4f::getIdentity(),
	                               GeneralDrawMod(), m_eval, imods);

	// ImGui::InvisibleButton("TextureCanvasIB", canvas_size);



	// ImVec2 canvasMax = canvas_pos;
	// canvasMax.x += canvas_size.x;
	// canvasMax.y += canvas_size.y;

	if (kIsTexcoordStyleD3D) {
		ImGui::Image(m_frameTarget->getRenderTarget(0), ImVec2(canvas_size.x, canvas_size.y));
	} else {
		ImGui::Image(m_frameTarget->getRenderTarget(0), ImVec2(canvas_size.x, canvas_size.y), ImVec2(0, 1), ImVec2(1, 0));
	}

	if (ImGui::IsItemHovered()) {
		camera.update(is.IsKeyDown(Key_LAlt), is.IsKeyDown(Key_MouseLeft), is.IsKeyDown(Key_MouseMiddle), is.IsKeyDown(Key_MouseRight),
		              is.GetCursorPos());
	}
}

void ModelPreviewWindow::doMomentUI(MomentDataUI& moment) {
	ImGui::Separator();

	ImGui::Checkbox("Enabled", &moment.isEnabled);

	if (ImGui::Button("Pick##Moment UI")) {
		promptForModel(moment.modelAsset);
		if (isAssetLoaded(moment.modelAsset)) {
			moment.moment.model = &moment.modelAsset->asModel()->model;
		}
	}

	ImGui::SameLine();
	if (isAssetLoaded(moment.modelAsset))
		ImGui::Text("%s", moment.modelAsset->getPath().c_str());
	else
		ImGui::Text("Pick a Model");

	if (isAssetLoaded(moment.modelAsset)) {
		const auto fnListAnimations = [](void* vptrModel, int idx, const char** out) -> bool {
			if (idx == 0) {
				*out = "<Static Moment>";
				return true;
			}

			Model::Model* const model = (Model::Model*)vptrModel;
			*out = model->m_animations[idx - 1].curveName.c_str();
			return true;
		};

		if (ImGui::Combo("Animation", &moment.animationMagicIndex, fnListAnimations, (void*)&moment.modelAsset->asModel()->model,
		                 int(moment.modelAsset->asModel()->model.m_animations.size()) + 1)) {
			if (moment.animationMagicIndex == 0) {
				moment.moment.animationName = "";
			} else {
				int idx = moment.animationMagicIndex - 1;
				moment.moment.animationName = moment.modelAsset->asModel()->model.m_animations[idx].curveName;
			}
		}

		if (moment.animationMagicIndex != 0) {
			int const idx = moment.animationMagicIndex - 1;
			const Model::AnimationInfo& animInfo = moment.modelAsset->asModel()->model.m_animations[idx];

			ImGui::SliderFloat("time", &moment.moment.time, 0.f, animInfo.duration);
		}

		ImGui::SliderFloat("weight", &moment.moment.weight, 0.f, 1.f);
	}
}



void ModelPreviewWindow::update(SGEContext* const sgecon, const InputState& is) {
	if (isClosed()) {
		return;
	}

	if (ImGui::Begin(m_windowName.c_str(), &m_isOpened)) {
		AssetLibrary* const assetLib = getCore()->getAssetLib();

		if (m_frameTarget.IsResourceValid() == false) {
			m_frameTarget = sgecon->getDevice()->requestResource<FrameTarget>();
			m_frameTarget->create2D(64, 64);
		}

		ImGui::Columns(2);
		ImGui::BeginChild("sidebar_wnd");

		if (ImGui::Button("Pick##ModeToPreview")) {
			promptForModel(m_model);
			m_eval = EvaluatedModel();
			m_momentsUI.clear();
			if (m_model) {
				m_eval.initialize(assetLib, &m_model->asModel()->model);
			}
		}

		ImGui::SameLine();
		if (isAssetLoaded(m_model))
			ImGui::Text("%s", m_model->getPath().c_str());
		else
			ImGui::Text("Pick a Model");

		if (ImGui::CollapsingHeader("Animation")) {
			ImGui::Checkbox("Autoplay", &m_autoPlay);
			for (auto& momentUI : m_momentsUI) {
				ImGui::PushID(&momentUI);
				doMomentUI(momentUI);
				ImGui::PopID();
			}

			if (ImGui::Button("+")) {
				m_momentsUI.push_back(MomentDataUI());
			}
		}

		ImGui::EndChild();
		ImGui::NextColumn();
		if (m_model.get() != NULL) {
			// Update the evaluation moments.
			if (m_autoPlay) {
				float maxAnimationDuration = 0.f;
				int longestMomentIndex = -1;
				for (int t = 0; t < m_momentsUI.size(); ++t) {
					auto& momentUI = m_momentsUI[t];
					if (momentUI.animationMagicIndex != 0) {
						float const duration = momentUI.moment.model->m_animations[momentUI.animationMagicIndex - 1].duration;
						if (duration > maxAnimationDuration) {
							maxAnimationDuration = duration;
							longestMomentIndex = t;
						}

						momentUI.moment.time += ImGui::GetIO().DeltaTime;
					}
				}

				// Check if the animation should start over.
				if (longestMomentIndex >= 0)
					if (maxAnimationDuration < m_momentsUI[longestMomentIndex].moment.time) {
						for (auto& momentUI : m_momentsUI) {
							if (momentUI.animationMagicIndex != 0) {
								momentUI.moment.time = 0.f;
							}
						}
					}
			}

			m_moments.clear();
			for (auto& momentUI : m_momentsUI) {
				if (momentUI.isEnabled == false)
					continue;
				if (momentUI.moment.model == nullptr)
					continue;
				m_moments.push_back(momentUI.moment);
			}

			if (m_moments.empty())
				m_eval.evaluate("", 0.f);
			else
				m_eval.evaluate(m_moments);


			const ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
			ImVec2 canvas_size = ImGui::GetContentRegionAvail();

			if (canvas_size.y < 64)
				canvas_size.y = 64;

			ImVec2 texsize = canvas_size;

			if (texsize.x < 64)
				texsize.x = 64;
			if (texsize.y < 64)
				texsize.y = 64;

			if (m_frameTarget->getWidth() != texsize.x || m_frameTarget->getHeight() != texsize.y) {
				m_frameTarget->create2D((int)texsize.x, (int)texsize.y);
			}

			float ratio = (float)m_frameTarget->getWidth() / (float)m_frameTarget->getHeight();

			mat4f lookAt = camera.GetViewMatrix();
			mat4f proj = mat4f::getPerspectiveFovRH(deg2rad(45.f), ratio, 0.1f, 10000.f,
			                                        kIsTexcoordStyleD3D); // The Y flip for OpenGL is done by the modelviewer.

			sgecon->clearColor(m_frameTarget, 0, vec4f(0.f).data);
			sgecon->clearDepth(m_frameTarget, 1.f);

			QuickDraw& debugDraw = getCore()->getQuickDraw();

			RenderDestination rdest(sgecon, m_frameTarget, m_frameTarget->getViewport());

			debugDraw.drawWiredAdd_Grid(vec3f(0), vec3f::getAxis(0), vec3f::getAxis(2), 5, 5, 0xFF333733);
			debugDraw.drawWired_Execute(rdest, proj * lookAt, nullptr);

			getCore()->getModelDraw().draw(rdest, camera.eyePosition(), -camera.orbitPoint.normalized0(), proj * lookAt,
			                               mat4f::getIdentity(), GeneralDrawMod(), m_eval, InstanceDrawMods());

			ImGui::InvisibleButton("TextureCanvasIB", canvas_size);

			if (ImGui::IsItemHovered()) {
				camera.update(is.IsKeyDown(Key_LAlt), is.IsKeyDown(Key_MouseLeft), is.IsKeyDown(Key_MouseMiddle),
				              is.IsKeyDown(Key_MouseRight), is.GetCursorPos());
			}

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			ImVec2 canvasMax = canvas_pos;
			canvasMax.x += canvas_size.x;
			canvasMax.y += canvas_size.y;

			draw_list->AddImage(m_frameTarget->getRenderTarget(0), canvas_pos, canvasMax);
			draw_list->AddRect(canvas_pos, canvasMax, ImColor(255, 255, 255));
		}
	}
	ImGui::End();
}

} // namespace sge
