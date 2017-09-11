
#include "RenderDefinitions.hpp"

namespace Phoenix
{
	class IRenderBackend;

	namespace RenderFrontend 
	{
		void init(RenderInit* renderInit);
		void submitCommands();
		void swapBuffers();
		void exit();

		void tempSetProgram(ProgramHandle handle);
		IRenderBackend* getBackend();

		VertexBufferHandle createVertexBuffer(const VertexBufferFormat& format);
		
		IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data);
		
		ShaderHandle createShader(const char* source, Shader::Type shaderType);
		
		ProgramHandle createProgram(const Shader::List& shaders);

		UniformHandle createUniform(ProgramHandle program, const char* name, Uniform::Type type, const void* data = nullptr, size_t dataSize = 0);

		void setUniform(UniformHandle handle, const void* data, size_t dataSize);

		void drawIndexed(VertexBufferHandle vb, IndexBufferHandle ib, Primitive::Type primitives, uint32_t start, uint32_t count, StateGroup& state);
	};

	class IRenderer
	{
	public:
		virtual ~IRenderer() {};
		virtual void submit() = 0;
	};

	class ForwardRenderer : public IRenderer
	{
	public:
		// Get all lights
		// Get all Renderables
		// Produce commands
		// Hand self to Frontend
		// Frontend submits 

		virtual void submit() override
		{
		}

	private:
		// CommandBucket m_bucket;
	};
}