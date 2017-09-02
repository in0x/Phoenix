#pragma once

#include <stdint.h>
#include <vector>
#include "Windows.h"
#include "IRenderBackend.hpp"
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
		Uniform::Type m_type; // Storing setter function directly would be better
		GLuint m_location;
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
		virtual void createUniform(UniformHandle handle, const char* name, Uniform::Type type, const void* data) override;

		virtual void setVertexBuffer(VertexBufferHandle vb) override;
		virtual void setIndexBuffer(IndexBufferHandle ib) override;
		virtual void setProgram(ProgramHandle prog) override;
		virtual void setDepth(Depth::Type depth) override;
		virtual void setRaster(Raster::Type raster) override;
		virtual void setBlend(Blend::Type blend) override;
		virtual void setStencil(Stencil::Type stencil) override;
		void setUniform(UniformHandle handle, const void* data);

		virtual void drawLinear(Primitive::Type primitive, uint32_t count, uint32_t start) override;
		virtual void drawIndexed(Primitive::Type primitive, uint32_t count, uint32_t start) override;

	private:
		GlVertexBuffer m_vertexBuffers[RenderConstants::c_maxVertexBuffers]; // These vectors need replacing when I create custom allocators
		GlIndexBuffer m_indexBuffers[RenderConstants::c_maxIndexBuffers];
		GlShader m_shaders[RenderConstants::c_maxShaders];
		GlProgram m_programs[RenderConstants::c_maxPrograms];
		GlUniform m_uniforms[RenderConstants::c_maxUniforms];

		HWND m_owningWindow;
		HGLRC m_renderContext;
		HDC m_deviceContext;
	};
}