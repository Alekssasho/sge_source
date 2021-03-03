#include "Texture_gl.h"
#include "GraphicsCommon_gl.h"
#include "GraphicsInterface_gl.h"

namespace sge {

//---------------------------------------------------------------
// Texture
//---------------------------------------------------------------
bool TextureGL::create(const TextureDesc& desc, const TextureData initalData[], const SamplerDesc samplerDesc) {
	DumpAllGLErrors();

	// Cleanup the current state
	destroy();

	// obtain gl constat state
	GLContextStateCache* const glcon = getDevice<SGEDeviceImpl>()->GL_GetContextStateCache();

	// generate the texture and bind it
	glcon->GenTextures(1, &m_glTexture);
	m_desc = desc;
	const GLenum glTexTarget = TextureDesc_GetGLNativeTextureTartget(desc);
	DumpAllGLErrors();

	// bind the newly created texture
	//[TODO]tune a bit GL_TEXTURE0 value....
	glcon->BindTextureEx(glTexTarget, GL_TEXTURE0, m_glTexture);

	// [TODO][HACK][HOTFIX]
	// Currently OpenGL samplers aren't supported!
	// I'm adding a dummy texture sampler parameters.
	// That code MUST be removed in the future!!!
	// if(glTexTarget == GL_TEXTURE_2D)
	{
		GLenum samplerFilterMin = GL_NEAREST;
		GLenum samplerFilterMag = GL_NEAREST;
		if (samplerDesc.filter == TextureFilter::Min_Mag_Mip_Linear) {
			samplerFilterMin = desc.hasMipMaps() ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
			samplerFilterMag = GL_NEAREST;
		}
		if (samplerDesc.filter == TextureFilter::Min_Mag_Mip_Point) {
			samplerFilterMin = desc.hasMipMaps() ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
			samplerFilterMag = GL_NEAREST;
		}

		const auto getGLNativeAddressMode = [](TextureAddressMode::Enum mode) -> GLenum {
			if (mode == TextureAddressMode::Repeat)
				return GL_REPEAT;
			if (mode == TextureAddressMode::ClampEdge)
				return GL_CLAMP_TO_EDGE;
			return GL_CLAMP_TO_BORDER;
		};

		GLenum const xAddresMode = getGLNativeAddressMode(samplerDesc.addressModes[0]);
		DumpAllGLErrors();
		GLenum const yAddresMode = getGLNativeAddressMode(samplerDesc.addressModes[1]);
		DumpAllGLErrors();
		GLenum const zAddresMode = getGLNativeAddressMode(samplerDesc.addressModes[2]);
		DumpAllGLErrors();

		glTexParameteri(glTexTarget, GL_TEXTURE_MIN_FILTER, samplerFilterMin);
		DumpAllGLErrors();

		glTexParameteri(glTexTarget, GL_TEXTURE_MAG_FILTER, samplerFilterMag);
		DumpAllGLErrors();

		// Apply the texture sampler.
		glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_S, xAddresMode);
		glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_T, yAddresMode);
#ifndef __EMSCRIPTEN__
		glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_R, zAddresMode);
		glTexParameterfv(glTexTarget, GL_TEXTURE_BORDER_COLOR, samplerDesc.colorBorder);
#endif
		DumpAllGLErrors();
	}

	const bool isCompressed = TextureFormat::IsBC(desc.format);

	GLint glInternalFormat;
	GLenum glFormat, glType;
	TextureFormat_GetGLNative(desc.format, glInternalFormat, glFormat, glType);

	if (glTexTarget == GL_TEXTURE_2D) {
		//[TODO] See the hack above...
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		int width = desc.texture2D.width;
		int height = desc.texture2D.height;

		for (int iMipLevel = 0; iMipLevel < desc.texture2D.numMips; ++iMipLevel) {
			if (isCompressed == false) {
				// The next line is used in Emscripen debugging, as there is no way of breaking here.
				// SGE_DEBUG_LOG("glInternalFormat = %x glFormat = %x glType = %x\n", glInternalFormat, glFormat, glType);
				glTexImage2D(GL_TEXTURE_2D, iMipLevel, glInternalFormat, width, height,
				             0, // border
				             glFormat, glType, (initalData) ? initalData[iMipLevel].data : NULL);
			} else {
				sgeAssert(initalData);
				sgeAssert(initalData[iMipLevel].data != NULL);
				sgeAssert(initalData[iMipLevel].sliceByteSize > 0);

				glCompressedTexImage2D(GL_TEXTURE_2D, iMipLevel, glInternalFormat, width, height,
				                       0, // border
				                       int(initalData[iMipLevel].sliceByteSize), initalData[iMipLevel].data);
			}

			width = maxOf(width / 2, 1);
			height = maxOf(height / 2, 1);
		}

		DumpAllGLErrors();
	} else if (glTexTarget == GL_TEXTURE_2D_MULTISAMPLE) {
#if !defined(__EMSCRIPTEN__)
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, desc.texture2D.numSamples, glInternalFormat, desc.texture2D.width,
		                        desc.texture2D.height, GL_TRUE);

		DumpAllGLErrors();
#else
		sgeAssert(false);
#endif
	} else if (glTexTarget == GL_TEXTURE_CUBE_MAP) {
		const GLuint OpenGL_CubeFaceInices[6] = {
		    GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		};

		for (int iArray = 0; iArray < m_desc.textureCube.arraySize; ++iArray) {
#if !defined(__EMSCRIPTEN__)
			for (int iFace = 0; iFace < SGE_ARRSZ(OpenGL_CubeFaceInices); ++iFace) {
				// The width and the hight for this mip.
				int width = desc.texture2D.width;
				int height = desc.texture2D.height;
				for (int iMipLevel = 0; iMipLevel < m_desc.textureCube.numMips; ++iMipLevel) {
					const int faceInitalDataIndex =
					    iArray * 6 * m_desc.textureCube.numMips + iFace * m_desc.textureCube.numMips + iMipLevel;

					if (glTexTarget == GL_TEXTURE_CUBE_MAP_ARRAY) {
						sgeAssert(isCompressed == false); // Not supported yet!

						// Cube arrays.
						glTexImage3D(OpenGL_CubeFaceInices[iFace], iMipLevel, glInternalFormat, width, height, iArray,
						             0, // Border,
						             glFormat, glType, initalData[faceInitalDataIndex].data);
					} else if (glTexTarget == GL_TEXTURE_CUBE_MAP) {
						// Caution:
						// A single cube. If you think that glTexImage3D could be used for single texture case,
						// you are wrong as it only works on ATI/AMD cards!
						if (isCompressed) {
							sgeAssert(initalData);
							sgeAssert(initalData[iMipLevel].data != NULL);
							sgeAssert(initalData[iMipLevel].sliceByteSize > 0);

							glCompressedTexImage2D(OpenGL_CubeFaceInices[iFace], iMipLevel, glInternalFormat, width, height,
							                       0, // border
							                       int(initalData[faceInitalDataIndex].sliceByteSize),
							                       initalData[faceInitalDataIndex].data);
						} else {
							const void* const initalDataForFace = (initalData != nullptr) ? initalData[faceInitalDataIndex].data : nullptr;

							glTexImage2D(OpenGL_CubeFaceInices[iFace], iMipLevel, glInternalFormat, width, height,
							             0, // Border,
							             glFormat, glType, initalDataForFace);
						}
					} else {
						// Should never happen.
						sgeAssert(false);
					}

					// Check if there were any errors while initializing the texture.
					DumpAllGLErrors();

					// Compute the size of the next mip.
					width = maxOf(width / 2, 1);
					height = maxOf(height / 2, 1);
				}
			}
#endif
		}
	} else {
		// [TODO] Unimplemented texture type.
		sgeAssert(false);
		destroy();

		return false;
	}

	DumpAllGLErrors();

	return true;
}

void TextureGL::destroy() {
	GLContextStateCache* const glcon = getDevice<SGEDeviceImpl>()->GL_GetContextStateCache();

	if (m_glTexture != 0) {
		glcon->DeleteTextures(1, &m_glTexture);
		m_glTexture = 0;
	}
}

bool TextureGL::isValid() const {
	return m_glTexture != 0;
}

} // namespace sge
