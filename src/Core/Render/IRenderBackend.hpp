#pragma once
#include "RenderDefinitions.hpp"
#include "../Logger.hpp"

namespace Phoenix 
{
	// A RenderContext represents an object that can be used to perform actions 
	// on rendering API (e.g. OpenGL or DirectX).  
	class IRenderBackend // Maybe this can be replaced with Non-virtual interface -> Allows for a place for UniformCache
	{
	public: 
		virtual ~IRenderBackend() {}
		virtual void init(RenderInit* initValues) = 0;

		virtual uint32_t getMaxTextureUnits() = 0;
		virtual uint32_t getMaxUniformCount() = 0;

		virtual void createVertexBuffer(VertexBufferHandle handle, const VertexBufferFormat& format) = 0;
		virtual void createIndexBuffer(IndexBufferHandle handle, size_t size, uint32_t count, const void* data) = 0;
		virtual void createShader(ShaderHandle handle, const char* source, Shader::Type shaderType) = 0;
		virtual void createProgram(ProgramHandle handle, const Shader::List& shaders) = 0;
		virtual void createTexture() = 0;
		virtual void createFrameBuffer() = 0;
		virtual void createUniform(ProgramHandle programHandle, UniformHandle uniformHandle, const char* name, Uniform::Type type) = 0;
		
		virtual void setVertexBuffer(VertexBufferHandle vb) = 0;
		virtual void setIndexBuffer(IndexBufferHandle ib) = 0;
		virtual void setProgram(ProgramHandle prog) = 0; 
		virtual void setDepth(Depth::Type depth) = 0;
		virtual void setRaster(Raster::Type raster) = 0;
		virtual void setBlend(Blend::Type blend) = 0;
		virtual void setStencil(Stencil::Type stencil) = 0;
		virtual void setUniform(UniformHandle handle, const void* data) = 0;

		virtual void drawLinear(Primitive::Type primitive, uint32_t count, uint32_t start) = 0;
		virtual void drawIndexed(Primitive::Type primitive, uint32_t count, uint32_t start) = 0;

		//TODO(Phil): Destruction functions / release of resources on self destruction

		virtual void swapBuffer() = 0;
	};
}