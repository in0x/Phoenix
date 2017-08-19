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

	struct GlUniform
	{
		Uniform::Type m_type;

	};

	class WGlRenderContext : public IRenderContext
	{
	public:
		explicit WGlRenderContext(HWND owningWindow);
		virtual ~WGlRenderContext();
		virtual void init() override;
		virtual void swapBuffer() override;

		virtual VertexBufferHandle createVertexBuffer(const VertexBufferFormat& format) override;
		virtual IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data) override;
		virtual ShaderHandle createShader(const char* source, Shader::Type shaderType) override;
		virtual ProgramHandle createProgram(const Shader::List& shaders) override;
		virtual TextureHandle createTexture() override;
		virtual FrameBufferHandle createFrameBuffer() override;
		virtual UniformHandle createUniform(const char* name, Uniform::Type type) override;

		virtual void setVertexBuffer(VertexBufferHandle vb) override;
		virtual void setIndexBuffer(IndexBufferHandle ib) override;
		virtual void setProgram(ProgramHandle prog) override;
		
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