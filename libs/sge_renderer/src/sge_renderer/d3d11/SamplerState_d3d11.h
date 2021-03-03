#pragma once 

#include "include_d3d11.h"
#include "sge_renderer/renderer/renderer.h"

namespace sge {

struct SamplerStateD3D11 : public SamplerState
{
	SamplerStateD3D11() { destroy(); }
	~SamplerStateD3D11() { destroy(); }

	bool create(const SamplerDesc& desc) final;

	const SamplerDesc& getDesc() const final { return m_cachedDesc; }

	void destroy() final;
	bool isValid() const final;

	ID3D11SamplerState* D3D11_GetResource() const { return m_samplerState; }

protected : 

	SamplerDesc m_cachedDesc;
	TComPtr<ID3D11SamplerState> m_samplerState;
};


}
