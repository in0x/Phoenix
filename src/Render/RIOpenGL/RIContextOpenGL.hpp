#pragma once

#include <Render/RIContext.hpp>

namespace Phoenix
{
	struct RIOpenGLResourceStore;
	struct TextureBind;
	class GlVertexBuffer;
	class GlIndexBuffer;
	class GlProgram;

	class RIContextOpenGL : public IRIContext
	{
	public:
		RIContextOpenGL(const RIOpenGLResourceStore* resources);

		virtual void drawLinear(EPrimitive primitives, uint32_t count, uint32_t start) override;

		virtual void drawLinear(VertexBufferHandle vbHandle, EPrimitive primitives, uint32_t count, uint32_t startIndex = 0) override;

		virtual void drawIndexed(VertexBufferHandle vbHandle, IndexBufferHandle ibHandle, EPrimitive primitives, uint32_t count = 0, uint32_t startIndex = 0) override;

		virtual void bindShaderProgram(ProgramHandle programHandle) override;

		virtual void bindUniform(UniformHandle uniformHandle, const void* data) override;

		virtual void bindVertexBuffer(VertexBufferHandle vbHandle) override;

		virtual void bindIndexBuffer(IndexBufferHandle ibHandle) override;

		virtual void clearRenderTargetColor(RenderTargetHandle rtHandle, const RGBA& clearColor) override;

		virtual void clearRenderTargetDepth(RenderTargetHandle rtHandle) override;

		virtual void uploadTextureData(Texture2DHandle handle, const void* data) override;

		virtual void uploadTextureData(TextureCubeHandle handle, ETextureCubeSide side, const void* data) override;

		virtual void bindTexture(UniformHandle samplerHandle, Texture2DHandle texHandle) override;

		virtual void bindTexture(UniformHandle samplerHandle, TextureCubeHandle texHandle) override;

		virtual void unbindTextures() override;

		virtual void bindRenderTarget(RenderTargetHandle handle) override;

		virtual void bindDefaultRenderTarget() override;

		virtual void setDepthTest(EDepth state) override;

		virtual void setDepthWrite(EDepth state) override;

		virtual void setBlendState(const BlendState& state) override;

		virtual void clearColor() override;

		virtual void clearDepth() override;

		virtual void endPass() override;

		virtual uint32_t getMaxTextureUnits() const override;

		virtual void bindConstantBufferToLocation(ConstantBufferHandle cbHandle, uint32_t location) override;

		virtual void updateConstantBuffer(ConstantBufferHandle cbHandle, const void* data, size_t numBytes, size_t offsetBytes = 0) override;

	private:
		void bindTextureBase(UniformHandle samplerHandle, const TextureBind& binding);

		const RIOpenGLResourceStore* m_resources;

		struct BoundState
		{
			const GlVertexBuffer* vertexbuffer = nullptr;
			const GlIndexBuffer* indexbuffer = nullptr ;
			const GlProgram* program = nullptr;
			uint8_t activeTextures = 0;

			enum { MAX_ACTIVE_CONSTANT_BUFFERS = 64 };

			ConstantBufferHandle cbBindings[MAX_ACTIVE_CONSTANT_BUFFERS];

		} m_boundState;

		uint32_t m_maxTextureUnits;
	};
}