#pragma once

#include <stdint.h>
#include <vector>
#include "Windows.h"
#include "IRenderContext.hpp"
#include "../OpenGL.hpp"

namespace Phoenix
{
	HANDLE(VertexBufferHandle, uint16_t)
	HANDLE(IndexBufferHandle, uint16_t)
	HANDLE(ShaderHandle, uint16_t)
	HANDLE(ProgramHandle, uint16_t)
	HANDLE(TextureHandle, uint16_t)
	HANDLE(FrameBufferHandle, uint16_t)

	struct GlVertexBuffer
	{
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

		virtual VertexBufferHandle createVertexBuffer(uint32_t size, const void* data) override;
		virtual IndexBufferHandle createIndexBuffer(uint32_t size, const void* data) override;
		virtual ShaderHandle createShader(const char* source, Shader::Type shaderType) override;
		virtual ProgramHandle createProgram(const Shader::List& shaders) override;
		virtual TextureHandle createTexture() override;
		virtual FrameBufferHandle createFrameBuffer() override;
		
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