#pragma once

#include "opengl_include.h"
#include "sge_renderer/renderer/renderer.h"

namespace sge {

struct QueryGL : public Query {
	QueryGL() {}
	~QueryGL() { destroy(); }

	bool create(QueryType::Enum const queryType) final;

	virtual void destroy() final;
	virtual bool isValid() const final;

	QueryType::Enum getType() const { return m_queryType; }

	GLuint GL_GetResource() { return m_glQuery; }

  private:
	GLuint m_glQuery = 0;
	QueryType::Enum m_queryType;
};

} // namespace sge
