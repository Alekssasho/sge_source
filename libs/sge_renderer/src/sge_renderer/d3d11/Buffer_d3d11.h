#pragma once

#include "include_d3d11.h"
#include "sge_renderer/renderer/renderer.h"

namespace sge {

//-------------------------------------------------------------------
// BufferD3D11
//-------------------------------------------------------------------
struct BufferD3D11 : public Buffer {
	BufferD3D11() {}
	~BufferD3D11() { destroy(); }

	bool create(const BufferDesc& desc, const void* const pInitalData) final;

	void destroy() final;
	bool isValid() const final;

	const BufferDesc& getDesc() const final { return m_bufferDesc; }

	void* map(const Map::Enum map, SGEContext* pDevice = nullptr);
	void unMap(SGEContext* pDevice = nullptr);


	ID3D11Buffer* D3D11_GetResource() const { return m_d3d11_buffer; }

  private:
	BufferDesc m_bufferDesc; // Buffer description.
	TComPtr<ID3D11Buffer> m_d3d11_buffer;
};

} // namespace sge
