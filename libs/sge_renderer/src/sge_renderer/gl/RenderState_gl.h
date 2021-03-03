#pragma once

#include "opengl_include.h"
#include "sge_renderer/renderer/renderer.h"

namespace sge {

//-----------------------------------------------------------------
// RasterizerStateGL 
//-----------------------------------------------------------------
struct RasterizerStateGL : public RasterizerState
{
	RasterizerStateGL() { }
	~RasterizerStateGL() { destroy(); }

	bool create(const RasterDesc& desc) final;

	virtual void destroy() final;
	virtual bool isValid() const final;

	const RasterDesc& getDesc() const final { return m_bufferedDesc; }

private:

	RasterDesc m_bufferedDesc;
	bool m_isValid = false;
};

//-----------------------------------------------------------------
// DepthStencilStateGL 
//-----------------------------------------------------------------
struct DepthStencilStateGL : public DepthStencilState
{
	DepthStencilStateGL() { }
	~DepthStencilStateGL() { destroy(); }

	bool create(const DepthStencilDesc& desc) final;

	virtual void destroy() final;
	virtual bool isValid() const final;

	const DepthStencilDesc& getDesc() const final { return m_bufferedDesc; }

private:

	DepthStencilDesc m_bufferedDesc;
	bool m_isValid = false;
};

//-----------------------------------------------------------------
// BlendStateGL
//-----------------------------------------------------------------
struct BlendStateGL : public BlendState
{
	BlendStateGL() {}
	~BlendStateGL() {}

	bool create(const BlendStateDesc& desc) final;

	virtual void destroy() final;
	virtual bool isValid() const final;

	const BlendStateDesc& getDesc() const final { return m_bufferedDesc; }

private:

	BlendStateDesc m_bufferedDesc;
	bool m_isValid = false;
};

}
