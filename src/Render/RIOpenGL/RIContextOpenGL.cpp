#include "RIContextOpenGL.hpp"
#include "RIOpenGLResourceStore.hpp"
#include "OpenGL.hpp"

#include <Render/RIDefs.hpp>
#include <Core/Logger.hpp>

#include <assert.h>

namespace Phoenix
{
	RIContextOpenGL::RIContextOpenGL(const RIOpenGLResourceStore* resources)
		: m_resources(resources)
		, m_maxTextureUnits(0)
	{
		m_maxTextureUnits = getMaxTextureUnits();

		GLint maxUboBindings = 0;
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUboBindings);
		assert(m_boundState.MAX_ACTIVE_CONSTANT_BUFFERS < maxUboBindings);
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
		case EPrimitive::TriangleStrips:
			return GL_TRIANGLE_STRIP;
		default:
			assert(false);
			return GL_INVALID_ENUM;
		}
	}

	uint32_t RIContextOpenGL::getMaxTextureUnits() const
	{
		GLint maxTextureUnits = 0;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
		return static_cast<uint32_t>(maxTextureUnits);
	}

	void RIContextOpenGL::drawLinear(EPrimitive primitives, uint32_t count, uint32_t start)
	{
		glDrawArrays(toGlPrimitive(primitives), start, count);
	}

	void RIContextOpenGL::drawLinear(VertexBufferHandle vbHandle, EPrimitive primitives, uint32_t count, uint32_t start)
	{
		bindVertexBuffer(vbHandle);
		glDrawArrays(toGlPrimitive(primitives), start, count);
	}

	void RIContextOpenGL::drawIndexed(VertexBufferHandle vbHandle, IndexBufferHandle ibHandle, EPrimitive primitives, uint32_t count, uint32_t startIndex)
	{
		bindVertexBuffer(vbHandle);

		const GlIndexBuffer* ib = m_resources->m_indexbuffers.getResource(ibHandle);
		bindIndexBuffer(ibHandle);

		if (0 == count)
		{
			count = static_cast<uint32_t>(ib->m_numElements);
		}

		glDrawElements(toGlPrimitive(primitives), count, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLubyte) * startIndex));
	}

	void RIContextOpenGL::bindShaderProgram(ProgramHandle programHandle)
	{
		const GlProgram* program = m_resources->m_programs.getResource(programHandle);
		m_boundState.program = program;
		glUseProgram(program->m_id);
	}

	void RIContextOpenGL::bindVertexBuffer(VertexBufferHandle vbHandle)
	{
		const GlVertexBuffer* vb = m_resources->m_vertexbuffers.getResource(vbHandle);
		m_boundState.vertexbuffer = vb;
		glBindVertexArray(vb->m_id);
	}

	void RIContextOpenGL::bindIndexBuffer(IndexBufferHandle ibHandle)
	{
		const GlIndexBuffer* ib = m_resources->m_indexbuffers.getResource(ibHandle);
		m_boundState.indexbuffer = ib;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->m_id);
	}

	void setUniform(const GlUniform& uniform, const void* data, EUniformType type)
	{
		switch (type)
		{
		case EUniformType::Mat4:
		{
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

		assert(!checkGlErrorOccured());
	}

	bool isBasicUniformType(GLenum glType)
	{
		return (glType == GL_FLOAT
			|| glType == GL_INT
			|| glType == GL_FLOAT_VEC3
			|| glType == GL_FLOAT_VEC4
			|| glType == GL_FLOAT_MAT3
			|| glType == GL_FLOAT_MAT4);
	}

	bool isSamplerType(GLenum glType)
	{
		return (glType == GL_TEXTURE_2D
			|| glType == GL_TEXTURE_3D
			|| glType == GL_TEXTURE_CUBE_MAP
			|| glType == GL_TEXTURE_1D);
	}

	void RIContextOpenGL::bindUniform(UniformHandle uniformHandle, const void* data)
	{
		const RIUniform* uniform = m_resources->m_uniforms.getResource(uniformHandle);
		const GlProgram* program = m_boundState.program;
		assert(nullptr != program);

		GlUniform glUniform;
		bool bIsActive = program->m_activeUniforms.getUniformIfExisting(uniform->m_nameHash, program->m_id, glUniform);

		if (bIsActive)
		{
			assert(isBasicUniformType(glUniform.m_glType));
			setUniform(glUniform, data, uniform->m_type);
		}
		else
		{
			Logger::errorf("Uniform %s does not exist in currently bound program", uniform->m_debugName);
		}
	}

	struct TextureBind
	{
		const RITexture& texture;
		GLuint texID;
		GLenum texturetype;
	};

	GLenum getSamplerType(GLenum textureType)
	{
		switch (textureType)
		{
		case GL_TEXTURE_2D:
			return GL_SAMPLER_2D;
		case GL_TEXTURE_3D:
			return GL_SAMPLER_3D;
		case GL_TEXTURE_CUBE_MAP:
			return GL_SAMPLER_CUBE;
		case GL_TEXTURE_1D:
			return GL_SAMPLER_1D;
		default:
			assert(false);
			return GL_INVALID_ENUM;
		}
	}

	void RIContextOpenGL::bindTextureBase(UniformHandle samplerHandle, const TextureBind& binding)
	{
		const GlProgram* program = m_boundState.program;
		assert(nullptr != program);

		const RIUniform* uniform = m_resources->m_uniforms.getResource(samplerHandle);

		GlUniform glUniform;
		bool bIsActive = program->m_activeUniforms.getUniformIfExisting(uniform->m_nameHash, program->m_id, glUniform);

		if (!bIsActive)
		{
			Logger::errorf("Texture %s does not have an equivalent sampler in currently bound program", uniform->m_debugName);
			return;
		}

		assert(glUniform.m_glType == getSamplerType(binding.texturetype));
		assert(m_boundState.activeTextures < m_maxTextureUnits);

		glActiveTexture(GL_TEXTURE0 + m_boundState.activeTextures);
		glBindTexture(binding.texturetype, binding.texID);
		
		glUniform1iv(glUniform.m_location, glUniform.m_numElements, reinterpret_cast<const GLint*>(&m_boundState.activeTextures));
	
		m_boundState.activeTextures++;

		assert(!checkGlErrorOccured());
	}

	void RIContextOpenGL::bindTexture(UniformHandle samplerHandle, Texture2DHandle texHandle)
	{
		const GlTexture2D* texture = m_resources->m_texture2Ds.getResource(texHandle);
		bindTextureBase(samplerHandle, { *texture, texture->m_glTex.m_id, GL_TEXTURE_2D });
	}

	void RIContextOpenGL::bindTexture(UniformHandle samplerHandle, TextureCubeHandle texHandle)
	{
		const GlTextureCube* texture = m_resources->m_textureCubes.getResource(texHandle);
		bindTextureBase(samplerHandle, { *texture, texture->m_glTex.m_id, GL_TEXTURE_CUBE_MAP });
	}

	void RIContextOpenGL::unbindTextures()
	{
		for (uint8_t activeTexture = 0; activeTexture < m_boundState.activeTextures; ++activeTexture)
		{
			glActiveTexture(GL_TEXTURE0 + activeTexture);
			glBindTexture(GL_TEXTURE_2D, 0); 
		}

		m_boundState.activeTextures = 0;
	}

	void RIContextOpenGL::clearRenderTargetColor(RenderTargetHandle rtHandle, const RGBA& clearColor)
	{
		const GlFramebuffer* fb = m_resources->m_framebuffers.getResource(rtHandle);
		glBindFramebuffer(GL_FRAMEBUFFER, fb->m_id);

		uint8_t attachments = fb->m_colorAttachCount;
		for (uint8_t i = 0; i < attachments; ++i)
		{
			glClearBufferfv(GL_COLOR, i, (GLfloat*)&clearColor);
		}
	}

	void RIContextOpenGL::clearRenderTargetDepth(RenderTargetHandle rtHandle)
	{
		const GlFramebuffer* fb = m_resources->m_framebuffers.getResource(rtHandle);
		GLfloat clearValue = 1;
		glBindFramebuffer(GL_FRAMEBUFFER, fb->m_id);
		glClearBufferfv(GL_DEPTH, 0, &clearValue);
	}

	void RIContextOpenGL::uploadTextureData(Texture2DHandle handle, const void* data)
	{
		const GlTexture2D* texture = m_resources->m_texture2Ds.getResource(handle);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->m_glTex.m_id);

		glTexSubImage2D(GL_TEXTURE_2D,
			0,
			0, 0,
			texture->m_width, texture->m_height,
			texture->m_glTex.m_components,
			texture->m_glTex.m_dataType,
			data);

		if (texture->m_numMips > 0)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		assert(!checkGlErrorOccured());
	}

	void RIContextOpenGL::uploadTextureData(TextureCubeHandle handle, ETextureCubeSide side, const void* data)
	{
		const GlTextureCube* texture = m_resources->m_textureCubes.getResource(handle);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture->m_glTex.m_id);

		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<uint32_t>(side),
			0,
			0, 0,
			texture->m_size, texture->m_size,
			texture->m_glTex.m_components,
			texture->m_glTex.m_dataType,
			data);
		
		if (texture->m_numMips > 0)
		{
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}

		assert(!checkGlErrorOccured());
	}

	void RIContextOpenGL::endPass()
	{
		m_boundState.activeTextures = 0;
		assert(!checkGlErrorOccured());
	}

	void RIContextOpenGL::bindRenderTarget(RenderTargetHandle handle)
	{
		const GlFramebuffer* framebuffer = m_resources->m_framebuffers.getResource(handle);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->m_id);
	}

	void RIContextOpenGL::bindDefaultRenderTarget()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RIContextOpenGL::clearColor()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void RIContextOpenGL::clearDepth()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void RIContextOpenGL::setDepthTest(EDepth state)
	{
		if (EDepth::Enable == state)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	void RIContextOpenGL::setDepthWrite(EDepth state)
	{
		if (EDepth::Enable == state)
		{
			glDepthMask(GL_TRUE);
		}
		else
		{
			glDepthMask(GL_FALSE);
		}
	}

	GLenum toGlBlendFactor(EBlendFactor factor)
	{
		switch (factor)
		{
		case EBlendFactor::Zero:
			return GL_ZERO;
		case EBlendFactor::One:
			return GL_ONE;
		case EBlendFactor::SrcColor:
			return GL_SRC_COLOR;
		case EBlendFactor::OneMinusSrcColor:
			return GL_ONE_MINUS_SRC_COLOR;
		case EBlendFactor::SrcAlpha:
			return GL_SRC_ALPHA;
		case EBlendFactor::OneMinusSrcAlpha:
			return GL_ONE_MINUS_SRC_ALPHA;
		case EBlendFactor::DstColor:
			return GL_DST_COLOR;
		case EBlendFactor::OneMinusDstColor:
			return GL_ONE_MINUS_DST_COLOR;
		case EBlendFactor::DstAlpha:
			return GL_DST_ALPHA;
		case EBlendFactor::OneMinusDstAlpha:
			return GL_ONE_MINUS_DST_ALPHA;
		default:
			assert(false);
			return GL_INVALID_ENUM;
		}
	}

	GLenum toGlBlendOp(EBlendOp op)
	{
		switch (op)
		{
		case EBlendOp::Add:
			return GL_FUNC_ADD;
		case EBlendOp::Subtract_SD:
			return GL_FUNC_SUBTRACT;
		case EBlendOp::Subtract_DS:
			return GL_FUNC_REVERSE_SUBTRACT;
		case EBlendOp::Min:
			return GL_MIN;
		case EBlendOp::Max:
			return GL_MAX;
		default:
			assert(false);
			return GL_INVALID_ENUM;
		}
	}

	void RIContextOpenGL::setBlendState(const BlendState& state)
	{
		if (EBlend::Disable == state.m_enabeld)
		{
			glDisable(GL_BLEND);
			return;
		}

		glEnable(GL_BLEND);

		glBlendEquationSeparate(toGlBlendOp(state.m_blendOpRGB), toGlBlendOp(state.m_blendOpA));

		glBlendFuncSeparate(toGlBlendFactor(state.m_factorSrcRGB)
			, toGlBlendFactor(state.m_factorDstRGB)
			, toGlBlendFactor(state.m_factorSrcA)
			, toGlBlendFactor(state.m_factorDstA));
	}

	void RIContextOpenGL::bindConstantBufferToLocation(ConstantBufferHandle cbHandle, uint32_t location)
	{
		const GlConstantBuffer* cb = m_resources->m_constantBuffers.getResource(cbHandle);

		GlUniform glUniform;
		bool bIsActive = m_boundState.program->m_activeUniforms.getUniformIfExisting(cb->m_nameHash, m_boundState.program->m_id, glUniform);
		assert(cb->m_bufferSizeBytes == glUniform.m_numElements);
	
		glBindBufferBase(GL_UNIFORM_BUFFER, location, cb->m_id);
	}

	void RIContextOpenGL::updateConstantBuffer(ConstantBufferHandle cbHandle, const void* data, size_t numBytes, size_t offsetBytes)
	{
		const GlConstantBuffer* cb = m_resources->m_constantBuffers.getResource(cbHandle);

		glBindBuffer(GL_UNIFORM_BUFFER, cb->m_id);
		glBufferSubData(GL_UNIFORM_BUFFER, offsetBytes, cb->m_bufferSizeBytes, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}