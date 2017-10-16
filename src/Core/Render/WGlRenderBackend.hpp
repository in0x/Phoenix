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

	struct GlTexture
	{
		char m_name[RenderConstants::c_maxUniformNameLenght];
		GLuint m_id;
		GLenum m_format;
		GLenum m_components;
		GLenum m_dataType;
	};

	class WGlRenderInit : public RenderInit
	{
	public:
		explicit WGlRenderInit(HWND owningWindow, uint8_t msaaSamples = 0)
			: RenderInit(ERenderApi::Gl)
			, m_owningWindow(owningWindow)
			, m_msaaSamples(msaaSamples)
		{
		}

		HWND m_owningWindow;
		uint8_t m_msaaSamples;
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
		virtual void createTexture(TextureHandle handle, ETexture::Format format, const TextureDesc& description, const char* name) override;
		virtual void uploadTextureData(TextureHandle handle, const void* data, uint32_t width, uint32_t height) override;
		virtual void createFrameBuffer() override;
		virtual void createUniform(UniformHandle& uniformHandle, const char* name, EUniform::Type type) override;

		virtual void setState(const CStateGroup& state) override;

		virtual void clearFrameBuffer(FrameBufferHandle handle, EBuffer::Type bitToClear, RGBA clearColor) override;

		virtual void drawLinear(VertexBufferHandle vertexbuffer, EPrimitive::Type primitive, uint32_t count, uint32_t start) override;
		virtual void drawIndexed(VertexBufferHandle vertexBuffer, IndexBufferHandle indexBuffer, EPrimitive::Type primitive, uint32_t count, uint32_t start) override;

	private:
		UniformHandle WGlRenderBackend::addUniform();
		
		void registerActiveUniforms(ProgramHandle programHandle);

		void bindUniforms(ProgramHandle boundProgram, const UniformInfo* uniforms, size_t count);
		void bindTexture(const GlUniform& uniform, const GlTexture& texture);
		void bindTextures(ProgramHandle boundProgram, const UniformInfo* locations, const TextureHandle* handles, size_t count);
		void setUniformImpl(const GlUniform& uniform, const void* data);
		decltype(UniformHandle::idx) getUniform(ProgramHandle boundProgram, const UniformInfo& info) const;

		void initBasic(const WGlRenderInit& initValues);
		void initGlew();
		void checkMsaaSupport(const WGlRenderInit& initValues);
		void initWithMSAA(const WGlRenderInit& initValues);

		void setVertexBuffer(VertexBufferHandle vb);
		void setIndexBuffer(IndexBufferHandle ib);
		void setProgram(ProgramHandle prog);
		void setDepth(EDepth::Type depth);
		void setRaster(ERaster::Type raster);
		void setBlend(EBlend::Type blend);
		void setStencil(EStencil::Type stencil);

		GlVertexBuffer m_vertexBuffers[VertexBufferHandle::maxValue()];
		GlIndexBuffer m_indexBuffers[IndexBufferHandle::maxValue()];
		GlShader m_shaders[ShaderHandle::maxValue()];
		GlProgram m_programs[ProgramHandle::maxValue()];
		GlUniform m_uniforms[UniformHandle::maxValue()];
		GlTexture m_textures[TextureHandle::maxValue()];

		std::map<Hash, UniformHandle> m_uniformMap;
		
		HWND m_owningWindow;
		HGLRC m_renderContext;
		HDC m_deviceContext;
		size_t m_uniformCount;
		uint8_t m_msaaSupport;
		uint8_t m_activeTextures;
	};
}