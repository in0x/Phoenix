#pragma once

#include <stdint.h>
#include <vector>
#include "Windows.h"
#include "IRenderContext.hpp"
#include "../OpenGL.hpp"

namespace Phoenix
{
	struct GlVertexBuffer
	{
		VertexBufferFormat m_format; // This format can later be used to check the layout when binding to a shader.
		GLuint m_id;				 
	};

	struct GlIndexBuffer
	{
		GLuint m_id;
	};

	struct GlShader
	{
		GLuint m_id;
		GLenum m_shaderType;
	};

	struct GlProgram
	{
		GLuint m_id;
	};

	class WGlRenderContext : public IRenderContext
	{
	public:
		explicit WGlRenderContext(HWND owningWindow);
		virtual ~WGlRenderContext();
		virtual void init() override;
		virtual void swapBuffer() override;

		virtual VertexBufferHandle createVertexBuffer(VertexBufferFormat format) override;
		virtual IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data) override;
		virtual ShaderHandle createShader(const char* source, Shader::Type shaderType) override;
		virtual ProgramHandle createProgram(const Shader::List& shaders) override;
		virtual TextureHandle createTexture() override;
		virtual FrameBufferHandle createFrameBuffer() override;

		void tempUseVertexBuffer(VertexBufferHandle handle)
		{
			GlVertexBuffer buffer = m_vertexBuffers[handle.idx];
			glBindVertexArray(buffer.m_id);
		}
		
		void tempUseIdxBuffer(IndexBufferHandle handle)
		{
			GlIndexBuffer buffer = m_indexBuffers[handle.idx];
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.m_id);
		}

		void tempUseProgram(ProgramHandle handle)
		{
			GlProgram program = m_programs[handle.idx];
			glUseProgram(program.m_id);
		}

	private:
		std::vector<GlVertexBuffer> m_vertexBuffers; // These vectors need replacing when I create custom allocators
		std::vector<GlIndexBuffer> m_indexBuffers;
		std::vector<GlShader> m_shaders;
		std::vector<GlProgram> m_programs;

		HWND m_owningWindow;
		HGLRC m_renderContext;
		HDC m_deviceContext;
	};
}