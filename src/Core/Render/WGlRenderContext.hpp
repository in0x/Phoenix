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

		virtual uint32_t getMaxTextureUnits() override;
		virtual uint32_t getMaxUniformCount() override;

		virtual void alloc(VertexBufferHandle& out) override;
		virtual void alloc(IndexBufferHandle& out) override;
		virtual void alloc(ShaderHandle& out) override;
		virtual void alloc(ProgramHandle& out) override;
		virtual void alloc(TextureHandle& out) override;
		virtual void alloc(FrameBufferHandle& out) override;
		virtual void alloc(UniformHandle& out) override;

		virtual void createVertexBuffer(VertexBufferHandle handle, const VertexBufferFormat& format) override;
		virtual void createIndexBuffer(IndexBufferHandle handle, size_t size, uint32_t count, const void* data) override;
		virtual void createShader(ShaderHandle handle, const char* source, Shader::Type shaderType) override;
		virtual void createProgram(ProgramHandle handle, const Shader::List& shaders) override;
		virtual void createTexture() override;
		virtual void createFrameBuffer() override;
		virtual void createUniform(UniformHandle handle, const char* name, Uniform::Type type) override;

		virtual void setVertexBuffer(VertexBufferHandle vb) override;
		virtual void setIndexBuffer(IndexBufferHandle ib) override;
		virtual void setProgram(ProgramHandle prog) override;
		virtual void setDepth(Depth::Type depth) override;
		virtual void setRaster(Raster::Type raster) override;
		virtual void setBlend(Blend::Type blend) override;
		virtual void setStencil(Stencil::Type stencil) override;

		virtual void drawLinear(Primitive::Type primitive, uint32_t count, uint32_t start) override;
		virtual void drawIndexed(Primitive::Type primitive, uint32_t count, uint32_t start) override;

		void tempUseProgram(ProgramHandle handle);

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