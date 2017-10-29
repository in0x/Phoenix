#include "RIContextOpenGL.hpp"
#include "RenderInitWGL.hpp"
#include "OpenGL.hpp"

#include "../Logger.hpp"
#include "../glew/wglew.h"
#include "../Windows/Win32Window.hpp"

#include "Windows.h"

#include <assert.h>

namespace Phoenix
{
	
//
//	void RIContextOpenGL::swapBuffer()
//	{
//		SwapBuffers(m_wglDetails->m_deviceContext);
//	}
//
//	uint32_t RIContextOpenGL::getMaxTextureUnits() const
//	{
//		GLint maxTextureUnits;
//		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
//		return static_cast<uint32_t>(maxTextureUnits);
//	}
//
//	uint32_t RIContextOpenGL::getMaxUniformCount() const
//	{
//		GLint maxUniforms;
//		glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &maxUniforms);
//		return static_cast<uint32_t>(maxUniforms);
//	}
//
//	
//

//
//	GLenum toGlFormat(ETexture::Format format)
//	{
//		static GLenum formats[] = { GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_CUBE_MAP };
//		return formats[format];
//	}
//
//	GLint toGlComponents(ETexture::Components components)
//	{
//		static GLenum glComponents[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA, GL_DEPTH_COMPONENT };
//		return glComponents[components];
//	}
//
//	GLenum toGlFilter(ETexture::Filter filter)
//	{
//		static GLenum glFilters[] = { GL_NEAREST, GL_LINEAR };
//		return glFilters[filter];
//	}
//
//	void RIContextOpenGL::uploadTextureData(TextureHandle& handle, const void* data, uint32_t width, uint32_t height)
//	{
//		GlTexture2D& tex = m_textures[handle.textureIdx];
//		tex.m_dataType = GL_UNSIGNED_BYTE;
//
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(tex.m_format, tex.m_id);
//
//		GLenum format = tex.m_format;
//
//		if (tex.m_format == GL_TEXTURE_CUBE_MAP)
//		{
//			format = GL_TEXTURE_CUBE_MAP_POSITIVE_X + tex.m_cubeface;
//			tex.m_cubeface++;
//		}
//
//		glTexImage2D(format,
//			0,
//			tex.m_components,
//			width, height,
//			0,
//			tex.m_components,
//			tex.m_dataType,
//			data);
//
//		checkGlError();
//	}
//
//	void RIContextOpenGL::createTexture(TextureHandle& handle, ETexture::Format format, const TextureDesc& desc, const char* name)
//	{
//		GlTexture2D& tex = m_textures[handle.textureIdx];
//
//		GLenum eformat = toGlFormat(format);
//		GLenum ecomponents = toGlComponents(desc.components);
//
//		tex.m_format = eformat;
//		tex.m_components = ecomponents;
//
//		glGenTextures(1, &tex.m_id);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(eformat, tex.m_id);
//
//		if (desc.width % 2 == 0)
//		{
//			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
//		}
//		else
//		{
//			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//		}
//
//		glTexParameterf(eformat, GL_TEXTURE_MAG_FILTER, toGlFilter(desc.magFilter));
//		glTexParameterf(eformat, GL_TEXTURE_MIN_FILTER, toGlFilter(desc.minFilter));
//
//		glTexParameteri(eformat, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(eformat, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//		if (format > ETexture::Tex2D)
//		{
//			glTexParameterf(eformat, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//		}
//
//		if (format == ETexture::CubeMap)
//		{
//			tex.m_cubeface = 0;
//		}
//
//		checkGlError();
//	}
//
//	GLenum toGlAttachment(ERenderAttachment::Type type)
//	{
//		static GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_STENCIL_ATTACHMENT, GL_DEPTH_ATTACHMENT };
//		return attachments[type];
//	}
//
//	void RIContextOpenGL::createRenderTarget(RenderTargetHandle& handle, const RenderTargetDesc& desc)
//	{
//		GlFramebuffer& fb = m_framebuffers[handle.idx];
//		fb.m_renderAttachments = desc.attachment;
//
//		glCreateFramebuffers(1, &fb.m_id);
//
//		if (desc.attachment & ERenderAttachment::Color)
//		{
//			glGenTextures(1, &fb.m_colorTex);
//			glBindTexture(GL_TEXTURE_2D, fb.m_colorTex);
//			glTexStorage2D(fb.m_colorTex, 1, GL_RGBA8, desc.width, desc.height);
//			glNamedFramebufferTexture(fb.m_id, GL_COLOR_ATTACHMENT0, fb.m_colorTex, 0);
//		}
//
//		if (desc.attachment & ERenderAttachment::Stencil)
//		{
//			glGenTextures(1, &fb.m_stencilTex);
//			glBindTexture(GL_TEXTURE_2D, fb.m_stencilTex);
//			glTexStorage2D(fb.m_stencilTex, 1, GL_STENCIL_INDEX8, desc.width, desc.height);
//			glNamedFramebufferTexture(fb.m_id, GL_STENCIL_ATTACHMENT, fb.m_stencilTex, 0);
//		}
//
//		if (desc.attachment & ERenderAttachment::Depth)
//		{
//			glGenTextures(1, &fb.m_depthTex);
//			glBindTexture(GL_TEXTURE_2D, fb.m_depthTex);
//			glTexStorage2D(fb.m_depthTex, 1, GL_DEPTH_COMPONENT32F, desc.width, desc.height);
//			glNamedFramebufferTexture(fb.m_id, GL_DEPTH_ATTACHMENT, fb.m_depthTex, 0);
//		}
//	}
//
//	// NOTE(Phil): Textures return type int because the locations map to 
//	// texture units.
//	EUniform::Type toUniformType(GLenum glType)
//	{
//		switch (glType)
//		{
//		case GL_FLOAT_VEC3:
//		{
//			return EUniform::Vec3;
//		} break;
//		case GL_FLOAT_VEC4:
//		{
//			return EUniform::Vec4;
//		} break;
//		case GL_FLOAT_MAT3:
//		{
//			return EUniform::Mat3;
//		} break;
//		case GL_INT:
//		case GL_SAMPLER_1D:
//		case GL_SAMPLER_2D:
//		case GL_SAMPLER_3D:
//		case GL_SAMPLER_CUBE:
//		{
//			return EUniform::Int;
//		} break;
//		case GL_FLOAT_MAT4:
//		{
//			return EUniform::Mat4;
//		} break;
//		case GL_FLOAT:
//		{
//			return EUniform::Float;
//		} break;
//		default:
//		{
//			Logger::error("Trying to convert invalid GlEnum to Uniform::Type");
//			assert(false);
//			return EUniform::Count;
//		} break;
//		}
//	}
//
//	bool checkIsBasicUniform(GLenum typeVal)
//	{
//		return (typeVal == GL_FLOAT
//			|| typeVal == GL_INT
//			|| typeVal == GL_FLOAT_VEC3
//			|| typeVal == GL_FLOAT_VEC4
//			|| typeVal == GL_FLOAT_MAT3
//			|| typeVal == GL_FLOAT_MAT4);
//	}
//
//	bool checkIsSampler(GLenum typeVal)
//	{
//		return (typeVal == GL_TEXTURE_2D
//			|| typeVal == GL_TEXTURE_3D
//			|| typeVal == GL_TEXTURE_CUBE_MAP
//			|| typeVal == GL_TEXTURE_1D);
//	}
//
//	UniformHandle RIContextOpenGL::addUniform()
//	{
//		UniformHandle handle = createUniformHandle();
//		handle.idx = m_uniformCount++;
//		return handle;
//	}
//

//
//	void RIContextOpenGL::createUniform(UniformHandle& uniformHandle, const char* name, EUniform::Type type)
//	{
//	}
//
//	void RIContextOpenGL::setUniformImpl(const GlUniform& uniform, const void* data)
//	{
//		switch (uniform.m_type)
//		{
//		case EUniform::Mat4:
//		{
//			glUniformMatrix4fv(uniform.m_location, uniform.m_size, false, static_cast<const GLfloat*>(data));
//		} break;
//		case EUniform::Vec3:
//		{
//			glUniform3fv(uniform.m_location, uniform.m_size, static_cast<const GLfloat*>(data));
//			checkGlError();
//		} break;
//		case EUniform::Vec4:
//		{
//			glUniform4fv(uniform.m_location, uniform.m_size, static_cast<const GLfloat*>(data));
//		} break;
//
//		case EUniform::Mat3:
//		{
//			glUniformMatrix3fv(uniform.m_location, uniform.m_size, false, static_cast<const GLfloat*>(data));
//		} break;
//		case EUniform::Float:
//		{
//			glUniform1fv(uniform.m_location, uniform.m_size, static_cast<const GLfloat*>(data));
//		} break;
//		case EUniform::Int:
//		{
//			glUniform1iv(uniform.m_location, uniform.m_size, static_cast<const GLint*>(data));
//		} break;
//		default:
//		{
//			Logger::error("Invalid uniform type used to set value");
//			assert(false);
//		} break;
//		};
//
//		checkGlError();
//	}
//
//	void RIContextOpenGL::setVertexBuffer(VertexBufferHandle vb)
//	{
//		GlVertexBuffer buffer = m_vertexBuffers[vb.idx];
//		glBindVertexArray(buffer.m_id);
//	}
//
//	void RIContextOpenGL::setIndexBuffer(IndexBufferHandle ib)
//	{
//		GlIndexBuffer buffer = m_indexBuffers[ib.idx];
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.m_id);
//	}
//
//	void RIContextOpenGL::RIContextOpenGL::setProgram(ProgramHandle prog)
//	{
//		GlProgram program = m_programs[prog.idx];
//		glUseProgram(program.m_id);
//	}
//
//	void RIContextOpenGL::setDepth(EDepth::Type depth)
//	{
//		switch (depth)
//		{
//		case EDepth::Enable:
//		{
//			glEnable(GL_DEPTH_TEST);
//		} break;
//		case EDepth::Disable:
//		{
//			glDisable(GL_DEPTH_TEST);
//		} break;
//		default: break;
//		}
//	}
//
//	void RIContextOpenGL::setRaster(ERaster::Type raster)
//	{
//		Logger::warning(__LOCATION_INFO__ "not implemented!");
//	}
//
//	void RIContextOpenGL::setBlend(EBlend::Type blend)
//	{
//		Logger::warning(__LOCATION_INFO__ "not implemented!");
//	}
//
//	void RIContextOpenGL::setStencil(EStencil::Type stencil)
//	{
//		Logger::warning(__LOCATION_INFO__ "not implemented!");
//	}
//
//	GLenum getPrimitiveEnum(EPrimitive::Type type)
//	{
//		static constexpr GLuint glType[] =
//		{
//			GL_POINTS,
//			GL_LINES,
//			GL_TRIANGLES,
//			GL_TRIANGLE_STRIP,
//			GL_QUADS
//		};
//
//		return glType[type];
//	}
//
//	decltype(UniformHandle::idx) RIContextOpenGL::getUniform(ProgramHandle boundProgram, const UniformInfo& info) const
//	{
//		Hash name = info.nameHash;
//		Hash hashWithProgram = hashBytes(&boundProgram.idx, sizeof(decltype(ProgramHandle::idx)), name);
//
//		if (m_uniformMap.find(hashWithProgram) != m_uniformMap.end())
//		{
//			return m_uniformMap.at(hashWithProgram).idx;
//		}
//		else
//		{
//			return UniformHandle::invalidValue;
//		}
//	}
//
//	void RIContextOpenGL::bindUniforms(ProgramHandle boundProgram, const UniformInfo* uniforms, size_t count)
//	{
//		// NOTE(Phil): At some point, the bound uniforms should be cached so that we don't constantly
//		// recopy their data.
//
//		for (size_t i = 0; i < count; ++i)
//		{
//			const UniformInfo& info = uniforms[i];
//			auto uniformIdx = getUniform(boundProgram, info);
//
//			if (UniformHandle::invalidValue == uniformIdx)
//			{
//				Logger::errorf("Uniform %s does not exist in program with handle id %d", info.name, boundProgram.idx);
//				continue;
//			}
//
//			if (m_uniforms[uniformIdx].m_type == info.type)
//			{
//				setUniformImpl(m_uniforms[uniformIdx], info.data);
//			}
//			else
//			{
//				Logger::error("Uniform exists in program, but type is mismatched");
//			}
//		}
//
//		checkGlError();
//	}
//
//	void RIContextOpenGL::bindTexture(const GlUniform& uniform, const GlTexture2D& texture)
//	{
//		assert(m_activeTextures < getMaxTextureUnits());
//		glActiveTexture(GL_TEXTURE0 + m_activeTextures);
//		glBindTexture(texture.m_format, texture.m_id);
//		glUniform1i(uniform.m_location, m_activeTextures);
//
//		m_activeTextures++;
//	}
//
//	void RIContextOpenGL::bindTextures(ProgramHandle boundProgram, const UniformInfo* locations, const TextureHandle* handles, size_t count)
//	{
//		for (size_t i = 0; i < count; ++i)
//		{
//			const UniformInfo& info = locations[i];
//			auto uniformIdx = getUniform(boundProgram, info);
//
//			if (UniformHandle::invalidValue == uniformIdx)
//			{
//				Logger::errorf("Texture %s does not exist in program with handle id %d", info.name, boundProgram.idx);
//				continue;
//			}
//
//			if (m_uniforms[uniformIdx].m_type == EUniform::Int)
//			{
//				bindTexture(m_uniforms[uniformIdx], m_textures[handles[i].textureIdx]);
//			}
//			else
//			{
//				Logger::error("Uniform exists in program, but type is not a sampler type");
//			}
//		}
//
//		getGlErrorString();
//		checkGlError();
//	}
//
//	void RIContextOpenGL::setState(const CStateGroup& state)
//	{
//		setProgram(state.program);
//		setDepth(state.depth);
//		bindUniforms(state.program, state.uniforms, state.uniformCount);
//		bindTextures(state.program, state.textureLocations, state.textures, state.textureCount);
//		//setBlend(state.blend);
//		//setStencil(state.stencil);
//		//setRaster(state.raster);
//		m_activeTextures = 0;
//	}
//
//	void RIContextOpenGL::drawLinear(VertexBufferHandle vertexbuffer, EPrimitive::Type primitive, uint32_t count, uint32_t start)
//	{
//		setVertexBuffer(vertexbuffer);
//		glDrawArrays(getPrimitiveEnum(primitive), start, count);
//	}
//
//	void RIContextOpenGL::drawIndexed(VertexBufferHandle vertexBuffer, IndexBufferHandle indexBuffer, EPrimitive::Type primitive, uint32_t count, uint32_t start)
//	{
//		setVertexBuffer(vertexBuffer);
//		setIndexBuffer(indexBuffer);
//		glDrawElements(getPrimitiveEnum(primitive), count, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLubyte) * start));
//	}
//
//	void RIContextOpenGL::clearRenderTarget(RenderTargetHandle handle, EBuffer::Type bitToClear, RGBA clearColor)
//	{
//		// TODO(PHIL): use actual frambuffer here, 0 is default for window
//
//		switch (bitToClear)
//		{
//		case EBuffer::Color:
//		{
//			glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clearColor);
//		} break;
//		case EBuffer::Depth:
//		{
//			glClear(GL_DEPTH_BUFFER_BIT);
//		} break;
//		case EBuffer::Stencil:
//		{
//			glClear(GL_STENCIL_BUFFER_BIT);
//		} break;
//		}
//	}
}