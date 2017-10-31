#include "RIContextOpenGL.hpp"
#include "RIOpenGLResourceStore.hpp"
#include "OpenGL.hpp"

#include "../RenderDefinitions.hpp"
#include "../../Logger.hpp"

#include "../../Math/Matrix4.hpp"

#include <assert.h>

namespace Phoenix
{
	RIContextOpenGL::RIContextOpenGL(const RIOpenGLResourceStore* resources)
		: m_resources(resources)
	{
	}

	GLenum toGlPrimitive(EPrimitive primitive)
	{
		switch (primitive)
		{
		case EPrimitive::Points:
			return GL_POINTS;
		case EPrimitive::Lines:
			return GL_LINES;
		case EPrimitive::Triangles:
			return GL_TRIANGLES;
		case EPrimitive::Quads:
			return GL_QUADS;
		default:
			assert(false);
			return GL_INVALID_ENUM;
		}
	}

	void RIContextOpenGL::drawLinear(VertexBufferHandle vbHandle, EPrimitive primitives, uint32_t count, uint32_t start)
	{
		setVertexBuffer(vbHandle);
		glDrawArrays(toGlPrimitive(primitives), start, count);
	}

	void RIContextOpenGL::drawIndexed(VertexBufferHandle vbHandle, IndexBufferHandle ibHandle, EPrimitive primitives, uint32_t count, uint32_t startIndex)
	{
		setVertexBuffer(vbHandle);
	
		const GlIndexBuffer* ib = m_resources->m_indexbuffers.getResource(ibHandle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->m_id);
		
		if (0 == count)
		{
			count = ib->m_numElements;
		}
		
		glDrawElements(toGlPrimitive(primitives), count, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLubyte) * startIndex));
	}

	void RIContextOpenGL::setShaderProgram(ProgramHandle programHandle)
	{
		const GlProgram* program = m_resources->m_programs.getResource(programHandle);
		glUseProgram(program->m_id);
	}

	void RIContextOpenGL::setVertexBuffer(VertexBufferHandle vbHandle)
	{
		const GlVertexBuffer* vb = m_resources->m_vertexbuffers.getResource(vbHandle);
		glBindVertexArray(vb->m_id);
	}

	void RIContextOpenGL::setIndexBuffer(IndexBufferHandle ibHandle)
	{
		const GlIndexBuffer* ib = m_resources->m_indexbuffers.getResource(ibHandle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->m_id);
	}
	
	void setUniform(const GlUniform& uniform, const void* data, EUniformType type)
	{
		switch (type)
		{
		case EUniformType::Mat4:
		{
			const Matrix4* mat = static_cast<const Matrix4*>(data);
			glUniformMatrix4fv(uniform.m_location, uniform.m_numElements, false, static_cast<const GLfloat*>(data));
		} break;
		case EUniformType::Vec3:
		{
			glUniform3fv(uniform.m_location, uniform.m_numElements, static_cast<const GLfloat*>(data));
		} break;
		case EUniformType::Vec4:
		{
			glUniform4fv(uniform.m_location, uniform.m_numElements, static_cast<const GLfloat*>(data));
		} break;

		case EUniformType::Mat3:
		{
			glUniformMatrix3fv(uniform.m_location, uniform.m_numElements, false, static_cast<const GLfloat*>(data));
		} break;
		case EUniformType::Float:
		{
			glUniform1fv(uniform.m_location, uniform.m_numElements, static_cast<const GLfloat*>(data));
		} break;
		case EUniformType::Int:
		{
			glUniform1iv(uniform.m_location, uniform.m_numElements, static_cast<const GLint*>(data));
		} break;
		default:
		{
			Logger::error("Invalid uniform type used to set value");
			assert(false);
		} break;
		};

		checkGlErrorOccured();
	}

	void RIContextOpenGL::setProgramData(UniformHandle uniformHandle, ProgramHandle programHandle, const void* data)
	{
		const RIUniform* uniform = m_resources->m_uniforms.getResource(uniformHandle);
		const GlProgram* program = m_resources->m_programs.getResource(programHandle);

		GlUniform glUniform;
		bool bIsActive = m_resources->m_actualUniforms.getUniformIfExisting(uniform->m_nameHash, program->m_id, glUniform);
	
		if (bIsActive)
		{
			glUseProgram(program->m_id);
			setUniform(glUniform, data, uniform->m_type);
		}
	}

	void RIContextOpenGL::clearRenderTargetColor(RenderTargetHandle rtHandle, const RGBA& clearColor)
	{
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clearColor);
	}
	
	void RIContextOpenGL::clearRenderTargetDepth(RenderTargetHandle rtHandle)
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}

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