#pragma once

#include "include_d3d11.h"
#include "sge_renderer/renderer/renderer.h"

#include <map>

namespace sge {

struct ShadingProgramD3D11;

//----------------------------------------------------------------------------
// ShaderD3D11
//----------------------------------------------------------------------------
struct ShaderD3D11 : public Shader
{
	ShaderD3D11() { }
	~ShaderD3D11() { destroy(); }

	// Creates the shader using the native language for the API.
	bool createNative(const ShaderType::Enum type, const char* pCode, const char* const entryPoint);

	// Create the shader using the custom shading language.
	bool create(const ShaderType::Enum type, const char* pCode, const char* preapendedCode = NULL) final;

	virtual void destroy() override;
	virtual bool isValid() const override;

	const ShaderType::Enum getShaderType() const final { return m_shaderType; }

	ID3D10Blob* D3D11_GetByteCode() { return m_compiledBlob; }
	ID3D11DeviceChild* D3D11_GetShader() const { return m_dx11Shader; }
	ID3D11InputLayout* D3D11_GetInputLayoutForVertexDeclIndex(const VertexDeclIndex vertexDeclIdx);

private :

	ShaderType::Enum m_shaderType;
	std::string m_cachedCode;

	std::map<VertexDeclIndex, TComPtr<ID3D11InputLayout>> m_inputLayouts;

	TComPtr<ID3D11DeviceChild> m_dx11Shader;
	TComPtr<ID3D10Blob> m_compiledBlob;
};

}
