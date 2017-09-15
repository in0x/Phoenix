
#include "RenderDefinitions.hpp"

namespace Phoenix
{
	class IRenderBackend;

	/*
		Uniforms:
			When creating a program, store a map of all active uniforms and their location
			Store a uniform, its name, its datatype and its data in a seperate frontend cache
		
			When drawing, for all submitted uniforms, if not already uploaded, check if the uniform
			exists in the programs uniform map and if yes, upload the data

			We use two caches
				One that gets stored when creating a uniform using the frontend
				And one for each program that is used for a series of drawcalls (lets us temporarily check if we have already updated it)

			Really should get a memory allocator for uniform data
	
	*/

	struct UniformInfo
	{
		char name[RenderConstants::c_maxUniformNameLenght];
		Uniform::Type type;
		UniformHandle handle;
		void* data;
	};

	class UniformStore
	{
		UniformInfo* m_uniforms;

	};

	namespace RenderFrontend 
	{
		void init(RenderInit* renderInit);
		void submitCommands();
		void swapBuffers();
		void exit();
	
		IRenderBackend* getBackend();

		VertexBufferHandle createVertexBuffer(const VertexBufferFormat& format);
		
		IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data);
		
		ShaderHandle createShader(const char* source, Shader::Type shaderType);
		
		ProgramHandle createProgram(const Shader::List& shaders);

		UniformHandle createUniform(ProgramHandle program, const char* name, Uniform::Type type, const void* data = nullptr, size_t dataSize = 0);

		void setUniform(UniformHandle handle, const void* data, size_t dataSize);

		void drawIndexed(VertexBufferHandle vb, IndexBufferHandle ib, Primitive::Type primitives, uint32_t start, uint32_t count, StateGroup& state);

		void clearFrameBuffer(FrameBufferHandle frame, Buffer::Type bitToClear, RGBA color);
	};

	/*class IRenderer
	{
	public:
		virtual ~IRenderer() {};
		virtual void submit() = 0;
	};*/

	//struct ForwardRenderer 
	//{
	//public:
	//	// Get all lights
	//	// Get all Renderables
	//	// Produce commands
	//	// Hand self to Frontend
	//	// Frontend submits 

	//	/*
	//	void submit(const Mesh* mesh)
	//	{
	//		RenderFrontend::
	//	}
	//	
	//	*/

	//	CommandBucket createBucket;
	//	CommandBucket drawBucket;


	//};
}