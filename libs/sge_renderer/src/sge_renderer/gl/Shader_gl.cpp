#include <stdio.h>
#include "GraphicsCommon_gl.h"
#include <algorithm>

#include "GraphicsInterface_gl.h"
#include "sge_renderer/renderer/HLSLTranslator.h"

#include "Shader_gl.h"

namespace sge {

//------------------------------------------------------------------------
// VertexMapperGL
//------------------------------------------------------------------------
bool VertexMapperGL::create(const ShadingProgram* shadingProgram, const VertexDeclIndex vertexDeclIndex)
{
	destroy();

	m_vertexDeclIndex = vertexDeclIndex;

	//obtain the vertex shader needed attributes
	const std::vector<VertShaderAttrib>& vertShaderAttribs = ((ShadingProgramGL*)shadingProgram)->getReflection().inputVertices;
	const std::vector<VertexDecl>& vertexDecl = getDevice()->getVertexDeclFromIndex(vertexDeclIndex);

	bool succeeded = true;
	for(int t = 0; t < (int)vertShaderAttribs.size(); ++t)
	{
		const VertShaderAttrib& attrib = vertShaderAttribs[t];

		//find the matching attribute
		const auto declItr = std::find_if(vertexDecl.begin(), vertexDecl.end(), 
			[&attrib](const VertexDecl& decl) {
				bool doesTypeMatch = (decl.format == attrib.type);

				// The vertex could use an ineger as a packed float4 color, add this as a valid scenario.
				if(!doesTypeMatch 
					&& (attrib.type == UniformType::Float4)
					&& (decl.format == UniformType::Int_RGBA_Unorm_IA)) {
					doesTypeMatch = true;
				}

				const bool match = (decl.semantic == attrib.name) && doesTypeMatch;

				if(!match) {
					return false;
				}

				return true;
			}
		);

		//if the searched attribute isn't found break the process
		if(declItr == vertexDecl.end()) { 
			succeeded = false;
			break;
		}

		GL_AttribLayout layoutGL;

		layoutGL.bufferSlot = declItr->bufferSlot;
		layoutGL.index = attrib.attributeLocation;
		layoutGL.byteOffset = int(declItr->byteOffset);
		layoutGL.type = declItr->format;

		m_glVertexLayout.push_back(layoutGL);
	}

	if(succeeded == false) {
		sgeAssert(false);
		destroy();
		return false;
	}

	return true;
}

void VertexMapperGL::destroy()
{
	m_vertexDeclIndex = VertexDeclIndex_Null;
	m_glVertexLayout.clear();
}

bool VertexMapperGL::isValid() const
{
	return m_vertexDeclIndex != VertexDeclIndex_Null && m_glVertexLayout.size() != 0;
}

//-----------------------------------------------------------------------
// ShaderGL
//-----------------------------------------------------------------------
bool ShaderGL::createNative(const ShaderType::Enum type, const char* pCode, const char* const UNUSED(entryPoint))
{
	// Cleanup any previous state.
	destroy();

	//Vlidate the input data
	if(pCode == NULL) {
		return false;
	}

	///create the shader object
	m_shaderType = type;
	m_glShader = glCreateShader(ShaderType_GetGLNative(m_shaderType));

	// cache the code for debugging purpouses.
	m_cachedCode = pCode;

	const char* pCodes = {pCode};

	glShaderSource(m_glShader, 1, &pCodes, nullptr);
	glCompileShader(m_glShader);

	// Check for compilation errors.
	GLint isSuccessful = 0;
	glGetShaderiv(m_glShader, GL_COMPILE_STATUS, &isSuccessful);

	if(isSuccessful == 0)
	{
		// Obtain the error message.
		GLint logLenght = 0;// The lenght of the error message.
		glGetShaderiv(m_glShader, GL_INFO_LOG_LENGTH, &logLenght);

		if(logLenght != 0)
		{
			GLint temp;
			std::vector<char> log(logLenght + 1);
			glGetShaderInfoLog(m_glShader, logLenght, &temp, log.data());
			sgeAssert(false && "Native compilation FAILED:\n");
		}

		destroy();
		return false;
	}

	return true;
}

bool ShaderGL::create(
	const ShaderType::Enum type,
	const char* pCode, 
	const char* preapendedCode)
{
	std::string codeWithPreappend; 
	if(preapendedCode != NULL)
	{
		codeWithPreappend.reserve(strlen(pCode) + strlen(preapendedCode) + 1);

		codeWithPreappend += preapendedCode;
		codeWithPreappend += "\n";
		codeWithPreappend += pCode;

		pCode = codeWithPreappend.data();
	}

	std::string convertedCode, conversionErrors;
	
	if(translateHLSL(pCode, ShadingLanguage::GLSL, type, convertedCode, conversionErrors) == false)
	{
		sgeAssert("Failed compiling HLSLPArser Shader:\n");
		if((char*)conversionErrors.c_str())
		{
			printf((char*)conversionErrors.c_str());
		}
		//SGE_DEBUG_ERR("Shader code:\n");
		//SGE_DEBUG_ERR(pCode);
		sgeAssert(false);
		return false;
	}

	return createNative(type, convertedCode.c_str(), "");
}

void ShaderGL::destroy()
{
	if(m_glShader != 0) {
		glDeleteShader(m_glShader);
		m_glShader = 0;
	}

	m_cachedCode = std::string();
}

bool ShaderGL::isValid() const
{
	return m_glShader != 0;
}

}
