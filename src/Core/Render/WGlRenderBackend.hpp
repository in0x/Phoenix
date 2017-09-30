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
		EUniform::Type m_type; 
		GLuint m_location;
		GLint m_size;
		ProgramHandle m_program;
	};

	class WGlRenderInit : public RenderInit
	{
	public:
		explicit WGlRenderInit(HWND owningWindow, size_t resourceListMemoryBytes)
			: RenderInit(resourceListMemoryBytes, ERenderApi::Gl)
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

		virtual uint32_t getMaxTextureUnits() const override;
		virtual uint32_t getMaxUniformCount() const override;

		virtual void createVertexBuffer(VertexBufferHandle handle, const VertexBufferFormat& format) override;
		virtual void createIndexBuffer(IndexBufferHandle handle, size_t size, uint32_t count, const void* data) override;
		virtual void createShader(ShaderHandle handle, const char* source, EShader::Type shaderType) override;
		virtual void createProgram(ProgramHandle handle, const EShader::List& shaders) override;
		virtual void createTexture() override;
		virtual void createFrameBuffer() override;		
		virtual void createUniform(UniformHandle& uniformHandle, const char* name, EUniform::Type type) override;

		virtual void setVertexBuffer(VertexBufferHandle vb) override;
		virtual void setIndexBuffer(IndexBufferHandle ib) override;
		virtual void setProgram(ProgramHandle prog) override;
		virtual void setDepth(EDepth::Type depth) override;
		virtual void setRaster(ERaster::Type raster) override;
		virtual void setBlend(EBlend::Type blend) override;
		virtual void setStencil(EStencil::Type stencil) override;
		virtual void setUniform(ProgramHandle programHandle, UniformHandle uniformHandle, const void* data) override;
		virtual void setState(const CStateGroup& state) override;

		virtual void clearFrameBuffer(FrameBufferHandle handle, EBuffer::Type bitToClear, RGBA clearColor) override;

		virtual void drawLinear(EPrimitive::Type primitive, uint32_t count, uint32_t start) override;
		virtual void drawIndexed(EPrimitive::Type primitive, uint32_t count, uint32_t start) override;

	private:
		GlVertexBuffer m_vertexBuffers[VertexBufferHandle::maxValue()];
		GlIndexBuffer m_indexBuffers[IndexBufferHandle::maxValue()];
		GlShader m_shaders[ShaderHandle::maxValue()];
		GlProgram m_programs[ProgramHandle::maxValue()];
		GlUniform m_uniforms[UniformHandle::maxValue()];

		HWND m_owningWindow;
		HGLRC m_renderContext;
		HDC m_deviceContext;
		size_t m_uniformCount;
		std::map<Hash, UniformHandle>	 m_uniformMap;

		UniformHandle WGlRenderBackend::addUniform();	
		void registerActiveUniforms(ProgramHandle programHandle);
		void bindUniforms(ProgramHandle boundProgram, const UniformInfo* uniforms, size_t count);
	};
}