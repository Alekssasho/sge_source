#pragma once

#include "sge_core/sgecore_api.h"
#include "sge_renderer/renderer/renderer.h"
#include "sge_utils/utils/OptionPermutator.h"
#include "sge_utils/utils/StaticArray.h"

namespace sge {

//------------------------------------------------------------
// ShadingProgramPermuator
//------------------------------------------------------------
struct SGE_CORE_API ShadingProgramPermuator {
	struct Unform {
		int safetyIndex;
		const char* uniformName;
	};

	struct Permutation {
		GpuHandle<ShadingProgram> shadingProgram;
		std::vector<BindLocation> uniformLUT;

		/// Using the cached bind locations for each uniforms, binds the input data to the specified uniform
		/// in in the @uniforms.
		template <size_t N>
		void bind(StaticArray<BoundUniform, N>& uniforms, const int uniformEnumId, void* const dataPointer) const {
			sgeAssert(dataPointer != nullptr);

			if (uniformLUT[uniformEnumId].isNull() == false) {
				[[maybe_unused]] bool bindSucceeded = uniforms.push_back(BoundUniform(uniformLUT[uniformEnumId], (dataPointer)));
				sgeAssert(bindSucceeded);
				sgeAssert(uniforms.back().bindLocation.isNull() == false && uniforms.back().bindLocation.uniformType != 0);
			};
		}
	};

  public:
	bool createFromFile(SGEDevice* sgedev,
	                    const char* const fileName,
	                    const std::vector<OptionPermuataor::OptionDesc>& compileTimeOptions,
	                    const std::vector<Unform>& uniformsToCacheInLUT);

	bool create(SGEDevice* sgedev,
	            const char* const shaderCode,
	            const std::vector<OptionPermuataor::OptionDesc>& compileTimeOptions,
	            const std::vector<Unform>& uniformsToCacheInLUT);


	const OptionPermuataor getCompileTimeOptionsPerm() const { return compileTimeOptionsPermutator; }
	const std::vector<Permutation>& getShadersPerPerm() const { return perPermutationShadingProg; }

  private:
	OptionPermuataor compileTimeOptionsPermutator;
	std::vector<Permutation> perPermutationShadingProg;
};

} // namespace sge
