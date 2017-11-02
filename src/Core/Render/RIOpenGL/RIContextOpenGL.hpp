#pragma once

#include "../RIResourceHandles.h"
#include "../RenderDefinitions.hpp"

namespace Phoenix
{
	struct RIOpenGLResourceStore;
	struct TextureBind;
	class GlVertexBuffer;
	class GlIndexBuffer;
	class GlProgram;

	class RIContextOpenGL
	{
	public:
		RIContextOpenGL(const RIOpenGLResourceStore* resources);

		void drawLinear(VertexBufferHandle vbHandle, EPrimitive primitives, uint32_t count, uint32_t startIndex = 0);

		void drawIndexed(VertexBufferHandle vbHandle, IndexBufferHandle ibHandle, EPrimitive primitives, uint32_t count = 0, uint32_t startIndex = 0);

		void bindShaderProgram(ProgramHandle programHandle);

		void bindUniform(UniformHandle uniformHandle, const void* data);

		void bindVertexBuffer(VertexBufferHandle vbHandle);

		void bindIndexBuffer(IndexBufferHandle ibHandle);

		void clearRenderTargetColor(RenderTargetHandle rtHandle, const RGBA& clearColor);

		void clearRenderTargetDepth(RenderTargetHandle rtHandle);

		void uploadTextureData(Texture2DHandle handle, const void* data);

		void uploadTextureData(TextureCubeHandle handle, ETextureCubeSide side, const void* data);

		void bindTexture(Texture2DHandle handle);

		void bindTexture(TextureCubeHandle handle);

		void endFrame();

		uint32_t getMaxTextureUnits() const;

	private:
		void bindTextureBase(const TextureBind& binding);

		const RIOpenGLResourceStore* m_resources;

		struct BoundState
		{
			const GlVertexBuffer* vertexbuffer = nullptr;
			const GlIndexBuffer* indexbuffer = nullptr ;
			const GlProgram*	program = nullptr;
			uint8_t activeTextureCount = 0;
		} m_boundState;
	};

	//class RIContextOpenGL : public RIContext
	//{
	//public:
	//	RIContextOpenGL();
	//	virtual ~RIContextOpenGL();
	//	
	//	virtual void init(RenderInit* initValues) override;

	//	virtual uint32_t getMaxTextureUnits() const override;
	//	virtual uint32_t getMaxUniformCount() const override;

	//	virtual void uploadTextureData(TextureHandle& handle, const void* data, uint32_t width, uint32_t height) override;
	//	
	//	virtual void setState(const CStateGroup& state) override;
	//	
	//	virtual void clearRenderTarget(RenderTargetHandle handle, EBuffer::Type bitToClear, RGBA clearColor) override;

	//	virtual void drawLinear(VertexBufferHandle vertexbuffer, EPrimitive::Type primitive, uint32_t count, uint32_t start) override;
	//	virtual void drawIndexed(VertexBufferHandle vertexBuffer, IndexBufferHandle indexBuffer, EPrimitive::Type primitive, uint32_t count, uint32_t start) override;

	//	virtual void swapBuffer() override;

	//private:
	//	void setVertexBuffer(VertexBufferHandle vb);
	//	void setIndexBuffer(IndexBufferHandle ib);
	//	void setProgram(ProgramHandle prog);
	//	void setDepth(EDepth::Type depth);
	//	void setRaster(ERaster::Type raster);
	//	void setBlend(EBlend::Type blend);
	//	void setStencil(EStencil::Type stencil);

	//	UniformHandle addUniform();

	//	void registerActiveUniforms(ProgramHandle programHandle);

	//	void bindUniforms(ProgramHandle boundProgram, const UniformInfo* uniforms, size_t count);
	//	void bindTexture(const GlUniform& uniform, const GlTexture2D& texture);
	//	void bindTextures(ProgramHandle boundProgram, const UniformInfo* locations, const TextureHandle* handles, size_t count);
	//	void setUniformImpl(const GlUniform& uniform, const void* data);
	//	decltype(UniformHandle::idx) getUniform(ProgramHandle boundProgram, const UniformInfo& info) const;

	//	GlVertexBuffer m_vertexBuffers[VertexBufferHandle::maxValue()];
	//	GlIndexBuffer m_indexBuffers[IndexBufferHandle::maxValue()];
	//	GlShader m_shaders[ShaderHandle::maxValue()];
	//	GlProgram m_programs[ProgramHandle::maxValue()];
	//	GlUniform m_uniforms[UniformHandle::maxValue()];
	//	GlTexture2D m_textures[TextureHandle::maxValue()];
	//	GlFramebuffer m_framebuffers[RenderTargetHandle::maxValue()];

	//	std::map<Hash, UniformHandle> m_uniformMap;

	//	RIWGlDetails* m_wglDetails;

	//	size_t m_uniformCount;
	//	uint8_t m_activeTextures;
	//};
}