#pragma once

#include <string>
#include "sge_renderer/renderer/GraphicsCommon.h"

namespace sge {

// Translates the input D3D9 Style HLSL to the specified language.
// Does preprocessing (+ #include directives) using MCPP.
// Assumes that the vertex shader main function is named vsMain
// Assumes that the pixel shader main function is named psMain
bool translateHLSL(
	const char* const pCode, 
	const ShadingLanguage::Enum shadingLanguage,
	const ShaderType::Enum shaderType,
	std::string& result,
	std::string& compilationErrors);

}
