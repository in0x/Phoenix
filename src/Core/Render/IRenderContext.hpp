#pragma once
#include "Render.hpp"
#include "../Logger.hpp"

namespace Phoenix 
{
	// Splitup in frontend that handles all the shared stuff, such as creating handles, caching uniforms etc.
	// IRenderContext is backend which is used via commands. Allows us to pull alloc functions out RenderContext
	// where they dont belong.

	// A RenderContext represents an object that can be used to perform actions 
	// on rendering API (e.g. OpenGL or DirectX).  
	class IRenderContext // Maybe this can be replaced with Non-virtual interface -> Allows for a place for UniformCache
	{
	public: 
		virtual ~IRenderContext() {}
		virtual void init() = 0;

		template <class T>
		T alloc()
		{
			T handle;
			alloc(handle);
			return handle;
		}

		virtual uint32_t getMaxTextureUnits() = 0;
		virtual uint32_t getMaxUniformCount() = 0;

		virtual void alloc(VertexBufferHandle& out) = 0;
		virtual void alloc(IndexBufferHandle& out) = 0;
		virtual void alloc(ShaderHandle& out) = 0;
		virtual void alloc(ProgramHandle& out) = 0;
		virtual void alloc(TextureHandle& out) = 0;
		virtual void alloc(FrameBufferHandle& out) = 0;
		virtual void alloc(UniformHandle& out) = 0;

		virtual void createVertexBuffer(VertexBufferHandle handle, const VertexBufferFormat& format) = 0;
		virtual void createIndexBuffer(IndexBufferHandle handle, size_t size, uint32_t count, const void* data) = 0;
		virtual void createShader(ShaderHandle handle, const char* source, Shader::Type shaderType) = 0;
		virtual void createProgram(ProgramHandle handle, const Shader::List& shaders) = 0;
		virtual void createTexture() = 0;
		virtual void createFrameBuffer() = 0;
		virtual void createUniform(UniformHandle handle, const char* name, Uniform::Type type, const void* data) = 0;
		
		virtual void setVertexBuffer(VertexBufferHandle vb) = 0;
		virtual void setIndexBuffer(IndexBufferHandle ib) = 0;
		virtual void setProgram(ProgramHandle prog) = 0; 
		virtual void setDepth(Depth::Type depth) = 0;
		virtual void setRaster(Raster::Type raster) = 0;
		virtual void setBlend(Blend::Type blend) = 0;
		virtual void setStencil(Stencil::Type stencil) = 0;

		virtual void drawLinear(Primitive::Type primitive, uint32_t count, uint32_t start) = 0;
		virtual void drawIndexed(Primitive::Type primitive, uint32_t count, uint32_t start) = 0;

		//TODO(Phil): Destruction functions / release of resources on self destruction

		virtual void swapBuffer() = 0;
	};
}