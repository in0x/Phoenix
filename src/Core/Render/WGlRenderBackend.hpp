#pragma once

#include <vector>
#include <map>
#include "Windows.h"
#include "IRenderBackend.hpp"
#include "../OpenGL.hpp"

namespace Phoenix
{
	struct GlVertexBuffer
	{
		VertexBufferFormat m_format; 
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
		GLuint m_location;
		ProgramHandle m_program;
	};

	class WGlRenderInit : public RenderInit
	{
	public:
		explicit WGlRenderInit(HWND owningWindow) 
			: RenderInit(RenderApi::Gl)
			, m_owningWindow(owningWindow)
		{
		}

		HWND m_owningWindow;
	};

	namespace 
	{
		static constexpr size_t MAX_BUFFERS = 1024;
	}

	class WGlRenderBackend : public IRenderBackend
	{
	public:
		WGlRenderBackend();
		virtual ~WGlRenderBackend();
		virtual void init(RenderInit* initValues) override;
		virtual void swapBuffer() override;

		virtual uint32_t getMaxTextureUnits() override;
		virtual uint32_t getMaxUniformCount() override;

		virtual void createVertexBuffer(VertexBufferHandle handle, const VertexBufferFormat& format) override;
		virtual void createIndexBuffer(IndexBufferHandle handle, size_t size, uint32_t count, const void* data) override;
		virtual void createShader(ShaderHandle handle, const char* source, Shader::Type shaderType) override;
		virtual void createProgram(ProgramHandle handle, const Shader::List& shaders) override;
		virtual void createTexture() override;
		virtual void createFrameBuffer() override;		
		virtual void createUniform(ProgramHandle programHandle, UniformHandle uniformHandle, const char* name, Uniform::Type type) override;

		virtual void setVertexBuffer(VertexBufferHandle vb) override;
		virtual void setIndexBuffer(IndexBufferHandle ib) override;
		virtual void setProgram(ProgramHandle prog) override;
		virtual void setDepth(Depth::Type depth) override;
		virtual void setRaster(Raster::Type raster) override;
		virtual void setBlend(Blend::Type blend) override;
		virtual void setStencil(Stencil::Type stencil) override;
		virtual void setUniform(UniformHandle handle, const void* data) override;

		virtual void drawLinear(Primitive::Type primitive, uint32_t count, uint32_t start) override;
		virtual void drawIndexed(Primitive::Type primitive, uint32_t count, uint32_t start) override;

	private:
		GlVertexBuffer m_vertexBuffers[VertexBufferHandle::maxValue()]; 
		GlIndexBuffer m_indexBuffers[IndexBufferHandle::maxValue()];
		GlShader m_shaders[ShaderHandle::maxValue()];
		GlProgram m_programs[ProgramHandle::maxValue()];
		GlUniform m_uniforms[UniformHandle::maxValue()];

		HWND m_owningWindow;
		HGLRC m_renderContext;
		HDC m_deviceContext;
	};
}