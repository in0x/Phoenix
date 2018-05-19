#pragma once

#include "RIDefs.hpp"

namespace Phoenix
{
	class IRIContext
	{
	public:
		virtual void drawLinear(EPrimitive primitives, uint32_t count, uint32_t start) = 0;

		virtual void drawLinear(VertexBufferHandle vbHandle, EPrimitive primitives, uint32_t count, uint32_t startIndex = 0) = 0;

		virtual void drawIndexed(VertexBufferHandle vbHandle, IndexBufferHandle ibHandle, EPrimitive primitives, uint32_t count = 0, uint32_t startIndex = 0) = 0;

		virtual void bindShaderProgram(ProgramHandle programHandle) = 0;

		virtual void bindUniform(UniformHandle uniformHandle, const void* data) = 0;

		virtual void bindVertexBuffer(VertexBufferHandle vbHandle) = 0;

		virtual void bindIndexBuffer(IndexBufferHandle ibHandle) = 0;

		virtual void clearRenderTargetColor(RenderTargetHandle rtHandle, const RGBA& clearColor) = 0;

		virtual void clearRenderTargetDepth(RenderTargetHandle rtHandle) = 0;

		virtual void uploadTextureData(Texture2DHandle handle, const void* data) = 0;

		virtual void uploadTextureData(TextureCubeHandle handle, ETextureCubeSide side, const void* data) = 0;

		virtual void bindTexture(UniformHandle samplerHandle, Texture2DHandle texHandle) = 0;

		virtual void bindTexture(UniformHandle samplerHandle, TextureCubeHandle texHandle) = 0;

		virtual void unbindTextures() = 0;

		virtual void bindRenderTarget(RenderTargetHandle handle) = 0;

		virtual void bindDefaultRenderTarget() = 0;

		virtual void setDepthTest(EDepth state) = 0;

		virtual void setDepthWrite(EDepth state) = 0;

		virtual void setBlendState(const BlendState& state) = 0;

		virtual void clearColor() = 0;

		virtual void clearDepth() = 0;

		virtual void endPass() = 0;

		virtual uint32_t getMaxTextureUnits() const = 0;

		virtual void bindConstantBufferToLocation(ConstantBufferHandle cbHandle, uint32_t location) = 0;

		virtual void updateConstantBuffer(ConstantBufferHandle cbHandle, const void* data, size_t numBytes, size_t offsetBytes = 0) = 0;
	};
}

