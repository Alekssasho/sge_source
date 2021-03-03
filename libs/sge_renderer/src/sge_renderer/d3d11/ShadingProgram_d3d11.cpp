#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <d3dcompiler.h>

#include <algorithm>
#include "GraphicsInterface_d3d11.h"
#include "ShadingProgram_d3d11.h"
#include "Shader_d3d11.h"

namespace sge {

bool ShadingProgramD3D11::create(const char* const pVSCode, const char* const pPSCode, const char* const preAppendedCode)
{
	// Create the Vertex Shader...
	GpuHandle<Shader> vs = getDevice()->requestResource(ResourceType::Shader);
	bool createShader = vs->create(ShaderType::VertexShader, pVSCode, preAppendedCode);

	if(createShader == false)
	{
		return createShader;
	}

	// Create the Pixel Shader.
	GpuHandle<Shader> ps = getDevice()->requestResource(ResourceType::Shader);
	createShader = ps->create(ShaderType::PixelShader, pPSCode, preAppendedCode);

	if(createShader == false)
	{
		return createShader;
	}

	return create(vs, ps);
}

bool ShadingProgramD3D11::create(Shader* vertShdr, Shader* pixelShdr)
{
	//Clean up the current state
	destroy();

	const bool vertexShaderValid = vertShdr && vertShdr->isValid();
	const bool pixelShaderValid = pixelShdr && pixelShdr->isValid();

	if(vertexShaderValid == false || pixelShaderValid == false)
	{
		return false;
	}

	m_vertShdr = vertShdr;
	m_pixShadr = pixelShdr;

	bool const reflectonSucceeded = m_reflection.create(this);
	if(reflectonSucceeded == false) {
		return false;
	}

	for(int& slot : m_globalCBufferSlot) {
		slot = -1;
	}

	for(const auto itr : m_reflection.cbuffers.m_uniforms) {
		if(itr.second.name == "$Globals") {
			m_globalCBufferSlot[itr.second.d3d11_shaderType] = itr.second.d3d11_bindingSlot;
		}
	}


	return true;
}

void ShadingProgramD3D11::destroy()
{
	//drop shader usage
	m_vertShdr.Release();
	m_pixShadr.Release();
}

bool ShadingProgramD3D11::isValid() const 
{
	return m_vertShdr.IsResourceValid() || m_pixShadr.IsResourceValid();
}

ID3D11VertexShader* ShadingProgramD3D11::D3D11_GetVertexShader() const 
{
	sgeAssert(m_vertShdr.IsResourceValid());
	return (ID3D11VertexShader*)m_vertShdr.as<ShaderD3D11>()->D3D11_GetShader();
}

ID3D11PixelShader* ShadingProgramD3D11::D3D11_GetPixelShader() const
{
	sgeAssert(m_pixShadr.IsResourceValid()) ;
	return (ID3D11PixelShader*)m_pixShadr.as<ShaderD3D11>()->D3D11_GetShader();
}

}
