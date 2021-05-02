#pragma once

#include "sge_renderer/renderer/renderer.h"

namespace sge {

struct SamplerStateGL : public SamplerState {
	SamplerStateGL() { destroy(); }
	~SamplerStateGL() { destroy(); }

	bool create(const SamplerDesc& desc) final;

	const SamplerDesc& getDesc() const final { return m_cachedDesc; }

	void destroy() final;
	bool isValid() const final;


  protected:
	SamplerDesc m_cachedDesc;
};


} // namespace sge
