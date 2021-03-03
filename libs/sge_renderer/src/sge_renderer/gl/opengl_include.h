#pragma once

#include <memory>
#include "sge_utils/utils/TypeTraits.h"

#include <GL/glew.h>
#include <GL/gl.h>

#define SGE_GL_UNKNOWN GL_ZERO

namespace sge {

inline void DumpGLError(const GLenum opengl_error_code)
{
	// Those error message are directly taken form Khronos Group OpenGL documentation.
	switch(opengl_error_code)
	{
	case GL_NO_ERROR:
		sgeAssert(false && "[GL_NO_ERROR] No error has been recorded.The value of this symbolic constant is guaranteed to be 0.\n"); break;
	case GL_INVALID_ENUM:
		sgeAssert(false && "[GL_INVALID_ENUM] An unacceptable value is specified for an enumerated argument.The offending command is ignored and has no other side effect than to set the error flag.\n"); break;
	case GL_INVALID_VALUE:
		sgeAssert(false && "[GL_INVALID_VALUE] A numeric argument is out of range.The offending command is ignored and has no other side effect than to set the error flag.\n"); break;
	case GL_INVALID_OPERATION:
		sgeAssert(false && "[GL_INVALID_OPERATION] The specified operation is not allowed in the current state.The offending command is ignored and has no other side effect than to set the error flag.\n"); break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		sgeAssert(false && "[GL_INVALID_FRAMEBUFFER_OPERATION] The framebuffer object is not complete.The offending command is ignored and has no other side effect than to set the error flag.\n"); break;
	case GL_OUT_OF_MEMORY:
		sgeAssert(false && "[GL_OUT_OF_MEMORY] There is not enough memory left to execute the command.The state of the GL is undefined, except for the state of the error flags, after this error is recorded.\n"); break;
	case GL_STACK_UNDERFLOW:
		sgeAssert(false && "[GL_STACK_UNDERFLOW] An attempt has been made to perform an operation that would cause an internal stack to underflow.\n"); break;
	case GL_STACK_OVERFLOW:
		sgeAssert(false && "[GL_STACK_OVERFLOW] An attempt has been made to perform an operation that would cause an internal stack to overflow.\n"); break;
	default :
		// unknown error type
		sgeAssert(false && "Unknown OpenGL error!\n");
		sgeAssert(false);
	}
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
inline void DumpAllGLErrors()
{
	GLenum opengl_error_code = glGetError();
	while(opengl_error_code != GL_NO_ERROR)
	{
		// Dump the error message for the error.
		DumpGLError(opengl_error_code);
		sgeAssert(false);
		
		// Advance to the next error
		opengl_error_code = glGetError();
	}
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
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

	//Unknown unsupported texture type
	sgeAssert(false);
	return GL_NONE;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------

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

}
