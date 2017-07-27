#pragma once
#include <limits>
#include <stdint.h>
#undef max // TODO(Phil): Figure out how to stop this

namespace Phoenix
{

#define HANDLE(name, size) \
		struct name \
		{ \
			static const size invalidValue = std::numeric_limits<size>::max(); \
			size idx = invalidValue; \
			static bool isValid(name handle) { return handle.idx != invalidValue; } \
		}; \

	struct VertexBufferHandle;
	struct IndexBufferHandle;
	struct ShaderHandle;
	struct ProgramHandle;
	struct TextureHandle;
	struct FrameBufferHandle;

	namespace Shader
	{
		enum class Type
		{
			Vertex,
			Geometry,
			Fragment,
			Compute,
		};
	}

	class IRenderContext
	{
	public: 
		virtual void init() = 0;
		virtual VertexBufferHandle createVertexBuffer(uint32_t size, const void* data) = 0;
		virtual IndexBufferHandle createIndexBuffer(uint32_t size, const void* data) = 0;
		virtual ShaderHandle createShader(const char* source, Shader::Type shaderType) = 0;
		virtual ProgramHandle createProgram() = 0;
		virtual TextureHandle createTexture() = 0;
		virtual FrameBufferHandle createFrameBuffer() = 0;
		virtual void swapBuffer() = 0;
	};
}