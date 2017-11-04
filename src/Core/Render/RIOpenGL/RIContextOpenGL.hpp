#pragma once

#include "../RIDefs.hpp"

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

		void drawLinear(EPrimitive primitives, uint32_t count, uint32_t start);

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

		void bindRenderTarget(RenderTargetHandle handle);

		void bindDefaultRenderTarget();

		void clearColor();

		void clearDepth();

		void endPass();

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
}