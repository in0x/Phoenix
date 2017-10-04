#pragma once
#include "RenderDefinitions.hpp"

namespace Phoenix 
{
	// A RenderContext represents an object that can be used to perform actions 
	// on rendering API (e.g. OpenGL or DirectX).  
	class IRenderBackend // Maybe this can be replaced with Non-virtual interface -> Allows for a place for UniformCache
	{
	public: 
		virtual ~IRenderBackend() {}
		virtual void init(RenderInit* initValues) = 0;

		virtual uint32_t getMaxTextureUnits() const = 0;
		virtual uint32_t getMaxUniformCount() const = 0;

		virtual void createVertexBuffer(VertexBufferHandle handle, const VertexBufferFormat& format) = 0;
		virtual void createIndexBuffer(IndexBufferHandle handle, size_t size, uint32_t count, const void* data) = 0;
		virtual void createShader(ShaderHandle handle, const char* source, EShader::Type shaderType) = 0;
		virtual void createProgram(ProgramHandle handle, const EShader::List& shaders) = 0;
		virtual void createTexture(TextureHandle handle, const TextureDesc& description, const char* name) = 0;
		virtual void createFrameBuffer() = 0;
		virtual void createUniform(UniformHandle& uniformHandle, const char* name, EUniform::Type type) = 0;

		virtual void setVertexBuffer(VertexBufferHandle vb) = 0;
		virtual void setIndexBuffer(IndexBufferHandle ib) = 0;
		virtual void setProgram(ProgramHandle prog) = 0; 
		virtual void setDepth(EDepth::Type depth) = 0;
		virtual void setRaster(ERaster::Type raster) = 0;
		virtual void setBlend(EBlend::Type blend) = 0;
		virtual void setStencil(EStencil::Type stencil) = 0;
		virtual void setUniform(ProgramHandle programHandle, UniformHandle uniformHandle, const void* data) = 0;
		virtual void setState(const CStateGroup& state) = 0;

		virtual void clearFrameBuffer(FrameBufferHandle handle, EBuffer::Type bitToClear, RGBA clearColor) = 0;

		virtual void drawLinear(EPrimitive::Type primitive, uint32_t count, uint32_t start) = 0;
		virtual void drawIndexed(EPrimitive::Type primitive, uint32_t count, uint32_t start) = 0;

		//TODO(Phil): Destruction functions / release of resources on self destruction

		virtual void swapBuffer() = 0;
	};
}