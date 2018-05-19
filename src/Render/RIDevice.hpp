#pragma once

#include <stdint.h>
#include "RIDefs.hpp"

namespace Phoenix
{
	class VertexBufferFormat;
	class TextureDesc;
	class RenderTargetDesc;

	class IRIDevice
	{
	public:
		virtual ~IRIDevice() {}

		virtual VertexBufferHandle	 createVertexBuffer(const VertexBufferFormat& format) = 0;
		
		virtual IndexBufferHandle	 createIndexBuffer(size_t elementSizeBytes, size_t count, const void* data) = 0;
		
		virtual VertexShaderHandle	 createVertexShader(const char* source) = 0;
		
		virtual FragmentShaderHandle createFragmentShader(const char* source) = 0;
		
		virtual ProgramHandle		 createProgram(VertexShaderHandle vs, FragmentShaderHandle fs) = 0;
		
		virtual Texture2DHandle		 createTexture2D(const TextureDesc& desc) = 0;
		
		virtual TextureCubeHandle	 createTextureCube(const TextureDesc& desc) = 0;
		
		virtual RenderTargetHandle	 createRenderTarget(const RenderTargetDesc& desc) = 0;
		
		virtual UniformHandle		 createUniform(const char* name, EUniformType type, EUniformIsArray isArray = EUniformIsArray::False) = 0;

		virtual ConstantBufferHandle createConstantBuffer(const char* name, size_t bufferSizeBytes) = 0;
	};
}
