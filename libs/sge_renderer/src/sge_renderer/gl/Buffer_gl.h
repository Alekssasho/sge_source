#pragma once

#include "opengl_include.h"
#include "sge_renderer/renderer/renderer.h"

namespace sge {

//-------------------------------------------------------------------
// BufferGL
//-------------------------------------------------------------------
class BufferGL : public Buffer
{
public:

	BufferGL() { }
	~BufferGL() { destroy(); }

	bool create(const BufferDesc& desc, const void * const pInitalData) final;

	void destroy() final;
	bool isValid() const final;

	const BufferDesc& getDesc() const final { return m_bufferDesc; }

	void* map(const Map::Enum map, SGEContext* pDevice = nullptr);
	void unMap(SGEContext* pDevice = nullptr);

	GLuint GL_GetResource() const { return m_glBuffer; }
	GLenum GL_GetTargetBufferType() const;

private:
	
#if defined(__EMSCRIPTEN__)
	std::vector<char> m_emsc_mapBufferHelper;
#endif

	BufferDesc m_bufferDesc; // Buffer description.
	GLuint m_glBuffer = 0;
};

}
