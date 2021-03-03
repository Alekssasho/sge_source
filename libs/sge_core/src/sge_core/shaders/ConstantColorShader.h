#pragma once

#include "sge_core/sgecore_api.h"
#include "ShadingProgramPermuator.h"
#include "sge_utils/math/mat4.h"
#include "sge_core/model/EvaluatedModel.h"
#include "sge_utils/utils/optional.h"

namespace sge {

struct EvaluatedModel;
namespace Model {
	struct Mesh;
}

//------------------------------------------------------------
// ConstantColorShader
//------------------------------------------------------------
struct SGE_CORE_API ConstantColorShader {
  public:
	ConstantColorShader() = default;

	void draw(const RenderDestination& rdest,
	          const mat4f& projView,
	          const mat4f& preRoot,
	          const EvaluatedModel& model,
	          const vec4f& shadingColor);

	void drawGeometry(
	    const RenderDestination& rdest, const mat4f& projView, const mat4f& world, const Geometry& geometry, const vec4f& shadingColor);

  private:
	bool isInitialized = false;
	Optional<ShadingProgramPermuator> shadingPermut;
	StateGroup stateGroup;
};

} // namespace sge
