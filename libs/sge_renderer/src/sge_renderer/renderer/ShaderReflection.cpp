#include "ShaderReflection.h"

namespace sge {

BindLocation ShadingProgramRefl::findUniform(const char* const uniformName) const {
	{
		BindLocation bl;
		bl = numericUnforms.findUniform(uniformName);
		if (bl.isNull() == false)
			return bl;
	}

	{
		BindLocation bl;
		bl = cbuffers.findUniform(uniformName);
		if (bl.isNull() == false)
			return bl;
	}

	{
		BindLocation bl;
		bl = textures.findUniform(uniformName);
		if (bl.isNull() == false)
			return bl;
	}

	{
		BindLocation bl;
		bl = samplers.findUniform(uniformName);
		if (bl.isNull() == false)
			return bl;
	}

	return BindLocation();
}

} // namespace sge