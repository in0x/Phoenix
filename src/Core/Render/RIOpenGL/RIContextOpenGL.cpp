#include "RIContextOpenGL.hpp"
#include "RIOpenGLResourceStore.hpp"
#include "OpenGL.hpp"

#include "../RIDefs.hpp"
#include "../../Logger.hpp"

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

	uint32_t RIContextOpenGL::getMaxTextureUnits() const
	{
		GLint maxTextureUnits;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
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
			count = ib->m_numElements;
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

		checkGlErrorOccured();
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
		bool bIsActive = m_resources->m_actualUniforms.getUniformIfExisting(uniform->m_nameHash, program->m_id, glUniform);

		if (bIsActive)
		{
			assert(isBasicUniformType(glUniform.m_glType));
			setUniform(glUniform, data, uniform->m_type);
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

	void RIContextOpenGL::bindTextureBase(const TextureBind& binding)
	{
		const GlProgram* program = m_boundState.program;
		assert(nullptr != program);

		GlUniform glUniform;
		bool bIsActive = m_resources->m_actualUniforms.getUniformIfExisting(binding.texture.m_namehash, program->m_id, glUniform);

		if (!bIsActive)
		{
			Logger::errorf("Texture %d does not have an equivalent sampler in currently bound program", binding.texture.m_namehash);
			return;
		}

		assert(glUniform.m_glType == getSamplerType(binding.texturetype));
		assert(m_boundState.activeTextureCount < getMaxTextureUnits());

		glActiveTexture(GL_TEXTURE0 + m_boundState.activeTextureCount);
		glBindTexture(binding.texturetype, binding.texID);
		setUniform(glUniform, &m_boundState.activeTextureCount, EUniformType::Int);

		m_boundState.activeTextureCount++;
	}

	void RIContextOpenGL::bindTexture(Texture2DHandle handle)
	{
		const GlTexture2D* texture = m_resources->m_texture2Ds.getResource(handle);
		bindTextureBase({ *texture, texture->m_glTex.m_id, GL_TEXTURE_2D });
	}

	void RIContextOpenGL::bindTexture(TextureCubeHandle handle)
	{
		const GlTextureCube* texture = m_resources->m_textureCubes.getResource(handle);
		bindTextureBase({ *texture, texture->m_glTex.m_id, GL_TEXTURE_CUBE_MAP });
	}

	void RIContextOpenGL::clearRenderTargetColor(RenderTargetHandle rtHandle, const RGBA& clearColor)
	{
		const GlFramebuffer* fb = m_resources->m_framebuffers.getResource(rtHandle);
		glBindFramebuffer(GL_FRAMEBUFFER, fb->m_id);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clearColor);
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

		checkGlErrorOccured();
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

		checkGlErrorOccured();
	}

	void RIContextOpenGL::endPass()
	{
		m_boundState.activeTextureCount = 0;
		checkGlErrorOccured();
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
}