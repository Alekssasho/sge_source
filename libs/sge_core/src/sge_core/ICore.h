#pragma once

#include "CoreLog.h"
#include "sgecore_api.h"
#include "sge_utils/sge_utils.h"
#include "sge_utils/math/mat4.h"
#include "sge_renderer/renderer/renderer.h"
#include <string>

namespace sge {
struct AssetLibrary;
struct QuickDraw;
struct DebugDraw2;
struct BasicModelDraw;
struct SolidWireframeModelDraw;
struct InputState;

struct Gizmo3D;
struct Gizmo3DTranslation;
struct Gizmo3DRotation;
struct Gizmo3DScale;
struct Gizmo3DScaleVolume;

struct SGE_CORE_API ICore {
	// A group of commonly used by the editor graphics resources and render states.
	struct GraphicsResources {
		// Unform string indices.
		unsigned projViewWorld_strIdx = 0;
		unsigned color_strIdx = 0;

		GpuHandle<RasterizerState> RS_default;
		GpuHandle<RasterizerState> RS_defaultWireframe;
		GpuHandle<RasterizerState> RS_cullInverse;
		GpuHandle<RasterizerState> RS_noCulling;
		GpuHandle<RasterizerState> RS_defaultBackfaceCCW;
		GpuHandle<RasterizerState> RS_wireframeBackfaceCCW;

		GpuHandle<DepthStencilState> DSS_default;
		GpuHandle<DepthStencilState> DSS_default_lessEqual;
		GpuHandle<DepthStencilState> DSS_default_lessEqual_noWrite;
		GpuHandle<DepthStencilState> DSS_always_noTest;
		GpuHandle<DepthStencilState> DSS_noWrite_noTest;

		GpuHandle<BlendState> BS_backToFrontAlpha;
		GpuHandle<BlendState> BS_addativeColor;
	};

  public:
	ICore() = default;
	virtual ~ICore() = default;

	virtual void setup(SGEDevice* const sgedev) = 0;

	virtual void drawGizmo(const RenderDestination& rdest, const Gizmo3D& gizmo, const mat4f& projView) = 0;

	virtual void drawTranslationGizmo(const RenderDestination& rdest, const Gizmo3DTranslation& gizmo, const mat4f& projView) = 0;
	virtual void drawRotationGizmo(const RenderDestination& rdest, const Gizmo3DRotation& gizmo, const mat4f& projView) = 0;
	virtual void drawScaleGizmo(const RenderDestination& rdest, const Gizmo3DScale& gizmo, const mat4f& projView) = 0;
	virtual void drawScaleVolumeGizmo(const RenderDestination& rdest, const Gizmo3DScaleVolume& gizmo, const mat4f& projView) = 0;

	// Accessros to commonly used subsystems.
	virtual AssetLibrary* getAssetLib() = 0;
	virtual QuickDraw& getQuickDraw() = 0;
	virtual DebugDraw2& getDebugDraw2() = 0;
	virtual BasicModelDraw& getModelDraw() = 0;

	virtual SGEDevice* getDevice() = 0;
	virtual GraphicsResources& getGraphicsResources() = 0;

	virtual void setInputState(const InputState& is) = 0;
	virtual const InputState& getInputState() const = 0;
	virtual const FrameStatistics& getLastFrameStatistics() const = 0;
	virtual void setLastFrameStatistics(const FrameStatistics& stats) = 0;

	virtual CoreLog& getLog() = 0;
};

#if defined(SGE_USE_DEBUG)
// https://stackoverflow.com/questions/5588855/standard-alternative-to-gccs-va-args-trick
#define SGE_DEBUG_LOG(_str_msg_, ...) \
	{ sge::getCore()->getLog().write((_str_msg_), ##__VA_ARGS__); }
#define SGE_DEBUG_CHECK(_str_msg_, ...) \
	{ sge::getCore()->getLog().writeCheck((_str_msg_), ##__VA_ARGS__); }
#define SGE_DEBUG_ERR(_str_msg_, ...) \
	{ sge::getCore()->getLog().writeError((_str_msg_), ##__VA_ARGS__); }
#define SGE_DEBUG_WAR(_str_msg_, ...) \
	{ sge::getCore()->getLog().writeWarning((_str_msg_), ##__VA_ARGS__); }
#else
#define SGE_DEBUG_LOG(_str_msg_, ...) \
	{}
#define SGE_DEBUG_ERR(_str_msg_, ...) \
	{}
#define SGE_DEBUG_WAR(_str_msg_, ...) \
	{}
#endif

/// The global of the current module(dll, exe, ect.). However we could "borrow" another modules "ICore" and use theirs.
SGE_CORE_API ICore* getCore();
SGE_CORE_API void setCore(ICore* global);
} // namespace sge
