#define IMGUI_DEFINE_MATH_OPERATORS

#include "ATimeline.h"
#include "IconsForkAwesome/IconsForkAwesome.h"
#include "sge_core/DebugDraw.h"
#include "sge_core/ICore.h"
#include "sge_core/SGEImGui.h"
#include "sge_engine/EngineGlobal.h"
#include "sge_engine/GameInspector.h"
#include "sge_engine/GameWorld.h"
#include "sge_engine/InspectorCmd.h"
#include "sge_engine/PhysicsHelpers.h"
#include "sge_engine/traits/TraitRigidBody.h"
#include "sge_engine/typelibHelper.h"
#include "sge_engine/windows/PropertyEditorWindow.h"
#include "sge_utils/math/Box.h"
#include "sge_utils/utils/strings.h"
#include <vector>

namespace sge {

typedef vector_map<int, transf3d> vector_map_float_transf3d;

// clang-format off
DefineTypeId(vector_map_float_transf3d, 20'03'01'0013);
DefineTypeId(ATimeline, 20'03'01'0014);

ReflBlock() {
	ReflAddTypeWithName(vector_map_float_transf3d, "vector_map<int, transf3d>")
	    .member2<vector_map<int, transf3d>, std::vector<int>, &vector_map<int, transf3d>::getAllKeys,
	             &vector_map<int, transf3d>::serializationSetKeys>("keys")
	    .member2<vector_map<int, transf3d>, std::vector<transf3d>, &vector_map<int, transf3d>::getAllValues,
	             &vector_map<int, transf3d>::serializationSetValues>("values");

	ReflAddActor(ATimeline)
		ReflMember(ATimeline, m_isEnabled)
		ReflMember(ATimeline, targetActorId)
		ReflMember(ATimeline, keyFrames)
		ReflMember(ATimeline, moveObjectsOnTop)
		ReflMember(ATimeline, playbackSettings)
	;
}
// clang-format on

//--------------------------------------------------------
// TimelineWindow
//--------------------------------------------------------
struct TimelineWindow final : public IImGuiWindow {
	TimelineWindow(std::string windowName, ATimeline* const timeline)
	    : m_windowName(std::move(windowName)) {
		if (timeline) {
			m_inspector = timeline->getWorld()->getInspector();
			animatorId = timeline->getId();
			if (timeline->keyFrames.size() > 0) {
				newKeyTime = timeline->keyFrames.getAllKeys().back() + 1.f;
			}
		}
	}

	void initialize(const ATimeline* const animator);
	bool isClosed() override { return !m_isOpened; }
	void update(SGEContext* const sgecon, const InputState& is) override;
	const char* getWindowName() const override { return m_windowName.c_str(); }

  private:
	bool m_isOpened = true;
	GameInspector* m_inspector = nullptr;
	std::string m_windowName;



	ObjectId animatorId;
	float newKeyTime = 0.f;
	int m_lastRightClickedFrame = -1;
	Optional<transf3d> m_copiedFrame;
};


void TimelineWindow::update(SGEContext* const UNUSED(sgecon), const InputState& UNUSED(is)) {
	if (isClosed()) {
		return;
	}

	ATimeline* const timeline = (ATimeline*)m_inspector->getWorld()->getActorById(animatorId);
	if (timeline == nullptr) {
		m_isOpened = false;
		return;
	}

	timeline->isInEditMode = true;
	timeline->doesEditModeNeedsUpdate = false;

	if (ImGui::Begin(m_windowName.c_str(), &m_isOpened)) {
		ImGuiEx::Label("Frames Count");
		ImGui::InputInt("##Frame Count", &timeline->frameCount, 1, 1);
		float kFrameButtonWidthPixels = 32.f;
		float kTimelineHeightPixels = 64.f;
		const float kScrollBarHeigth = 24.f;
		const float totalTimelineWidthPixels = kFrameButtonWidthPixels * float(timeline->frameCount);

		const float FPS = float(timeline->framesPerSecond);
		const float frameLengthSeconds = 1.f / FPS;

		// Caution:
		// ImDrawList uses screen coordinates not window space,
		// while placing widgets happes in window space.
		// Below we try to use that "global" space for our logic.

		ImRect scrollBarRectScreen;
		scrollBarRectScreen.Min = ImGui::GetCursorScreenPos();
		scrollBarRectScreen.Max.x = scrollBarRectScreen.Min.x + ImGui::GetContentRegionAvail().x;
		scrollBarRectScreen.Max.y = scrollBarRectScreen.Min.y + kScrollBarHeigth;

		static float scrollBarXOffsetPixels = 0.f;
		ImGui::ScrollbarEx(scrollBarRectScreen, ImGui::GetID("##ScrollbarTimeline"), ImGuiAxis_X, &scrollBarXOffsetPixels,
		                   ImGui::GetContentRegionAvail().x, totalTimelineWidthPixels, ImDrawCornerFlags_All);

		const ImVec2 preTimelineCursorPosWindowSpace = ImGui::GetCursorPos() + ImVec2(0.f, kScrollBarHeigth);

		ImRect timelineRectScreen;
		timelineRectScreen.Min = ImGui::GetCursorScreenPos() + ImVec2(0.f, kScrollBarHeigth);
		timelineRectScreen.Max = timelineRectScreen.Min;
		timelineRectScreen.Max.y += kTimelineHeightPixels;
		timelineRectScreen.Max.x += totalTimelineWidthPixels;

		ImRect timelineClipBBox;
		timelineClipBBox = timelineRectScreen;
		timelineClipBBox.Max.x = timelineClipBBox.Min.x + ImGui::GetContentRegionAvail().x;

		int rightClickedSegment = -1;

		// Draw and handle timeline input.
		ImDrawList* const drawList = ImGui::GetWindowDrawList();
		drawList->PushClipRect(timelineClipBBox.Min, timelineClipBBox.Max);
		for (int iFrame = 0; iFrame < timeline->frameCount; ++iFrame) {
			// Compute the global space position of the frame rectangle.
			ImVec2 frameBBoxMinScreen = timelineRectScreen.Min;
			frameBBoxMinScreen.x += float(iFrame) * kFrameButtonWidthPixels - scrollBarXOffsetPixels;
			ImVec2 frameBBoxMaxScreen = frameBBoxMinScreen;
			frameBBoxMaxScreen.x += kFrameButtonWidthPixels;
			frameBBoxMaxScreen.y += kTimelineHeightPixels;

			// Color the frames in alternating colors to make them more easily visible.
			// If the frame is selected or being interacted, highlight it.
			int frameColor = iFrame % 2 ? 0xffAAAAAA : 0xff888888;
			if (m_lastRightClickedFrame == iFrame) {
				frameColor = 0xff008888;
			}

			// Draw the frame rectangle.
			drawList->AddRectFilled(frameBBoxMinScreen, frameBBoxMaxScreen, frameColor);

			// Check if there is a keyframe placed on this frame, if so display it.
			const bool hasKeyframe = timeline->keyFrames.find_element_index(iFrame) > -1;
			if (hasKeyframe) {
				// Draw something that marks that here we have a keyframe.
				ImVec2 textSize = ImGui::CalcTextSize(ICON_FK_KEY);
				drawList->AddText(frameBBoxMinScreen + ImVec2(textSize.x * 0.5f, kTimelineHeightPixels * 0.5f), 0xff0000ab, ICON_FK_KEY);
			}

			// Draw the number of the frame.
			{
				std::string frameNum = string_format("%d", iFrame);
				ImVec2 textSize = ImGui::CalcTextSize(frameNum.c_str());
				drawList->AddText(frameBBoxMinScreen + ImVec2(textSize.x * 0.5f, 0), 0xff000000, frameNum.c_str());
			}

			// Highlight the hovered frame if we aren't already interacting with another frame.
			if (m_lastRightClickedFrame == -1) {
				if (ImGui::IsMouseHoveringRect(frameBBoxMinScreen, frameBBoxMaxScreen, true)) {
					drawList->AddRect(frameBBoxMinScreen, frameBBoxMaxScreen, 0xff00ffff);

					if (ImGui::IsMouseClicked(1, false)) {
						// Remeber that we need to show the menu for this frame.
						rightClickedSegment = iFrame;
					}

					if (ImGui::IsMouseDown(0)) {
						// Evaluate the animation at this frame.
						timeline->doesEditModeNeedsUpdate = true;
						timeline->editModeFollowState.distanceAlongPath = float(iFrame) * frameLengthSeconds;
					}
				}
			}
		}
		drawList->PopClipRect();

		// Show the context menu if the user interacted (right clicked) on any frame.
		if (m_lastRightClickedFrame == -1 && rightClickedSegment != -1) {
			m_lastRightClickedFrame = rightClickedSegment;
			ImGui::OpenPopup("Timeline Frame Right Click Menu");
		}

		if (ImGui::BeginPopup("Timeline Frame Right Click Menu")) {
			const bool hasKeyframe = timeline->keyFrames.find_element_index(m_lastRightClickedFrame) > -1;

			if (ImGui::MenuItem(ICON_FK_PAPERCLIP " New Key")) {
				const Actor* const targetActor = m_inspector->getWorld()->getActorById(timeline->targetActorId);
				timeline->keyFrames[m_lastRightClickedFrame] = targetActor->getTransform();
			}

			if (hasKeyframe && ImGui::MenuItem(ICON_FK_FLOPPY_O " Copy")) {
				m_copiedFrame = timeline->keyFrames[m_lastRightClickedFrame];
			}

			if (hasKeyframe && ImGui::MenuItem(ICON_FK_SCISSORS " Cut")) {
			}

			if (m_copiedFrame.hasValue() && ImGui::MenuItem(ICON_FK_DOWNLOAD " Paste")) {
				timeline->keyFrames[m_lastRightClickedFrame] = m_copiedFrame.get();
			}

			if (hasKeyframe && ImGui::MenuItem(ICON_FK_TRASH " Delete Key")) {
				int keyFrameIndex = timeline->keyFrames.find_element_index(m_lastRightClickedFrame);
				if (keyFrameIndex != -1) {
					timeline->doesEditModeNeedsUpdate = true;
					timeline->keyFrames.eraseAtIndex(keyFrameIndex);
				}
			}

			ImGui::EndPopup();
		} else {
			m_lastRightClickedFrame = -1;
		}

		// Set cursor right below the timeline widget so newly created widgets will appear below it.
		ImGui::SetCursorPos(preTimelineCursorPosWindowSpace + ImVec2(0.f, kTimelineHeightPixels));
		ImGui::NewLine();
	}
	ImGui::End();

	timeline->isInEditMode = m_isOpened;
}

//--------------------------------------------------------
// ATimeline
//--------------------------------------------------------
AABox3f ATimeline::getBBoxOS() const {
	return AABox3f();
}

void ATimeline::create() {
	registerTrait(*(IActorCustomAttributeEditorTrait*)this);
	registerTrait(ttViewportIcon);

	ttViewportIcon.setTexture(getEngineGlobal()->getEngineAssets().getIconForObjectType(sgeTypeId(ATimeline)), true);
}


void ATimeline::postUpdate(const GameUpdateSets& u) {
	const float frameLengthSeconds = 1.f / framesPerSecond;

	if (keyFrames.size() == 0) {
		return;
	}

	Actor* const targetActor = getWorld()->getActorById(targetActorId);
	if (!targetActor) {
		return;
	}

	PathLengthFollow::State& state = (isInEditMode) ? editModeFollowState : followState;

	const bool shouldUpdate = (u.isPlaying() && !isInEditMode && m_isEnabled) || (isInEditMode && doesEditModeNeedsUpdate);

	if (shouldUpdate) {
		const float deltaTimeForUpdate = shouldUpdate ? u.dt : 0.f;

		const float animLengthRaw = getRawAnimationLength();
		float x = state.distanceAlongPath;
		state = PathLengthFollow::compute(animLengthRaw, deltaTimeForUpdate, getWorld(), playbackSettings, state);
		state.distanceAlongPath = x;

		// Find the keys that we need to use to compute the final transform.
		int iKey = 0;
		for (int frame = 1; frame < keyFrames.size(); ++frame) {
			if (keyFrames.getAllKeys()[frame] * frameLengthSeconds >= state.distanceAlongPath) {
				iKey = frame - 1;
				break;
			}
		}

		const transf3d oldTransform = targetActor->getTransform();
		transf3d newTransfrom = keyFrames.valueAtIdx(0);
		if (keyFrames.size() > 1) {
			const float t0 = keyFrames.keyAtIdx(iKey) * frameLengthSeconds;
			const float t1 = keyFrames.keyAtIdx(iKey + 1) * frameLengthSeconds;
			const float k = (state.distanceAlongPath - t0) / (t1 - t0);
			newTransfrom = lerp(keyFrames.valueAtIdx(iKey), keyFrames.valueAtIdx(iKey + 1), k);
		}

		const transf3d diffTransform = newTransfrom * oldTransform.inverseSimple();

		if (moveObjectsOnTop) {
			// Now find and move all actors that are on top of our target actor.
			vector_map<Actor*, FindActorsOnTopResult> actorsToMove;
			findActorsOnTop(actorsToMove, targetActor);

			for (auto pair : actorsToMove) {
				if (!pair.key() || pair.key() == targetActor) {
					continue;
				}
				bool applyRotation = true;
				const TraitRigidBody* const actorTraitRB = getTrait<TraitRigidBody>(pair.key());
				if (actorTraitRB) {
					const btVector3 actorAngFactor = actorTraitRB->getRigidBody()->getBulletRigidBody()->getAngularFactor();

					applyRotation &= isEpsEqual(actorAngFactor.getX(), 1.f);
					applyRotation &= isEpsEqual(actorAngFactor.getY(), 1.f);
					applyRotation &= isEpsEqual(actorAngFactor.getZ(), 1.f);
				}

				transf3d newActorTform = pair.key()->getTransform();
				if (applyRotation) {
					newActorTform = diffTransform * newActorTform;
				} else {
					newActorTform.p = (diffTransform * newActorTform).p;
				}

				pair.key()->setTransform(newActorTform, false);
			}
		}

		if (targetActor) {
			if (!isInEditMode || (isInEditMode && doesEditModeNeedsUpdate)) {
				targetActor->setTransform(newTransfrom, true);
			}
		}
	}
}

void ATimeline::doAttributeEditor(GameInspector* inspector) {
	MemberChain chain;

	chain.add(typeLib().findMember(&ATimeline::targetActorId));
	ProperyEditorUIGen::doMemberUI(*inspector, this, chain);
	chain.pop();

	chain.add(typeLib().findMember(&ATimeline::m_isEnabled));
	ProperyEditorUIGen::doMemberUI(*inspector, this, chain);
	chain.pop();

	chain.add(typeLib().findMember(&ATimeline::playbackSettings));
	ProperyEditorUIGen::doMemberUI(*inspector, this, chain);
	chain.pop();

	if (ImGui::Button("Edit")) {
		editModeFollowState = followState;
		getEngineGlobal()->addWindow(new TimelineWindow(string_format("Timeline of '%s'", this->getDisplayName().c_str()), this));
	}
}


} // namespace sge
