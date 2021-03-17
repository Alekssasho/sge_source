#pragma once

#include <memory>
#include "sge_utils/utils/TypeTraits.h"

#include <GL/glew.h>
#include <GL/gl.h>

#define SGE_GL_UNKNOWN GL_ZERO

namespace sge {

void DumpGLError(const GLenum opengl_error_code);
void DumpAllGLErrors();

inline GLenum GLUniformTypeToTextureType(const GLenum uniformType)
{
	switch(uniformType)
	{
		// 1D
		case GL_SAMPLER_1D:                    return GL_TEXTURE_1D;
		case GL_SAMPLER_1D_SHADOW:             return GL_TEXTURE_1D;
		case GL_INT_SAMPLER_1D:                return GL_TEXTURE_1D;
		case GL_UNSIGNED_INT_SAMPLER_1D:       return GL_TEXTURE_1D;

		case GL_SAMPLER_1D_ARRAY:              return GL_TEXTURE_1D_ARRAY;
		case GL_SAMPLER_1D_ARRAY_SHADOW:       return GL_TEXTURE_1D_ARRAY;
		case GL_INT_SAMPLER_1D_ARRAY:          return GL_TEXTURE_1D_ARRAY;
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return GL_TEXTURE_1D_ARRAY;

		// 2D
		case GL_SAMPLER_2D:                    return GL_TEXTURE_2D;
		case GL_SAMPLER_2D_SHADOW:             return GL_TEXTURE_2D;
		case GL_INT_SAMPLER_2D:                return GL_TEXTURE_2D;
		case GL_UNSIGNED_INT_SAMPLER_2D:       return GL_TEXTURE_2D;

		case GL_SAMPLER_2D_ARRAY:              return GL_TEXTURE_2D_ARRAY;
		case GL_SAMPLER_2D_ARRAY_SHADOW:       return GL_TEXTURE_2D_ARRAY;
		case GL_INT_SAMPLER_2D_ARRAY:          return GL_TEXTURE_2D_ARRAY;
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return GL_TEXTURE_2D_ARRAY;

		// 2D MultiSampled
		case GL_SAMPLER_2D_MULTISAMPLE:                    return GL_TEXTURE_2D_MULTISAMPLE;
		case GL_INT_SAMPLER_2D_MULTISAMPLE:                return GL_TEXTURE_2D_MULTISAMPLE;
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:       return GL_TEXTURE_2D_MULTISAMPLE;

		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:              return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
		case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:          return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;

		// Cube
		case GL_SAMPLER_CUBE:                        return GL_TEXTURE_CUBE_MAP;
		case GL_SAMPLER_CUBE_SHADOW:                 return GL_TEXTURE_CUBE_MAP;
		case GL_INT_SAMPLER_CUBE:                    return GL_TEXTURE_CUBE_MAP;
		case GL_UNSIGNED_INT_SAMPLER_CUBE:           return GL_TEXTURE_CUBE_MAP;

		case GL_SAMPLER_CUBE_MAP_ARRAY:              return GL_TEXTURE_CUBE_MAP;
		case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:       return GL_TEXTURE_CUBE_MAP;
		case GL_INT_SAMPLER_CUBE_MAP_ARRAY:          return GL_TEXTURE_CUBE_MAP;
		case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY: return GL_TEXTURE_CUBE_MAP;

		// 3D
		case GL_SAMPLER_3D:              return GL_TEXTURE_3D;
		case GL_INT_SAMPLER_3D:          return GL_TEXTURE_3D;
		case GL_UNSIGNED_INT_SAMPLER_3D: return GL_TEXTURE_3D;
	}

	// Unknown unsupported texture type.
	sgeAssert(false);
	return GL_NONE;
}

struct GLViewport
{
	GLViewport(GLint _x = 0, GLint _y = 0, GLsizei w = 0, GLsizei h = 0) :
		x(_x), y(_y), width(w), height(h)
	{}

	bool operator==(const GLViewport& other)
	{
		return x == other.x
			&& y == other.y
			&& width == other.width
			&& height == other.height;
	}

	bool operator!=(const GLViewport& other) {
		return !(*this == other);
	}

	GLint x;
	GLint y;
	GLsizei width;
	GLsizei height;
};

} // namespace sge
