
#include "RenderDefinitions.hpp"

namespace Phoenix
{
	class IRenderBackend;

	struct UniformInfo;

	namespace RenderFrontend
	{
		void init(RenderInit* renderInit);
		void submitCommands();
		void swapBuffers();
		void exit();

		VertexBufferHandle createVertexBuffer(const VertexBufferFormat& format);

		IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data);

		ShaderHandle createShader(const char* source, EShader::Type shaderType);

		ProgramHandle createProgram(const EShader::List& shaders);

		UniformHandle createUniform(const char* name, EUniform::Type type, const void* data = nullptr, size_t dataSize = 0);

		TextureHandle createTexture(const TextureDesc& desc, const void* data, ETexture::Format format, const char* samplerName);

		TextureHandle createCubemap(const TextureDesc& desc, const char* samplerName, const CubemapData& cubemap);

		void setUniform(UniformHandle handle, const void* data, size_t dataSize);

		void drawIndexed(VertexBufferHandle vb, IndexBufferHandle ib, EPrimitive::Type primitives, uint32_t start, uint32_t count, StateGroup& state);

		void drawLinear(VertexBufferHandle vb, EPrimitive::Type primitive, uint32_t count, uint32_t start, StateGroup& state);

		void clearRenderTarget(RenderTargetHandle frame, EBuffer::Type bitToClear, RGBA color);
	};
