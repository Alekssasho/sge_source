#pragma once

#include "include_d3d11.h"
#include "sge_renderer/renderer/renderer.h"

namespace sge {

struct QueryD3D11 : public Query
{
	QueryD3D11() {}
	~QueryD3D11() { destroy(); }

	bool create(QueryType::Enum const queryType) final;

	virtual void destroy() final;
	virtual bool isValid() const final;

	QueryType::Enum getType() const final { return m_queryType; }

	ID3D11Query* D3D11_GetResource() const { return m_d3d11_query; }

private : 

	QueryType::Enum m_queryType;
	TComPtr<ID3D11Query> m_d3d11_query;
};

}
