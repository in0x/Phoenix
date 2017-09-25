
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
		
		ShaderHandle createShader(const char* source, Shader::Type shaderType);
		
		ProgramHandle createProgram(const Shader::List& shaders);

		UniformHandle createUniform(const char* name, Uniform::Type type, const void* data = nullptr, size_t dataSize = 0);

		void* allocResourceList(size_t size, size_t alignment);
		
		template<class Resource, class... Resources>
		ResourceList<Resource> createResourceList(Resources... resources)
		{
			void* mem = allocResourceList(sizeof...(resources) * sizeof(Resource), alignof(Resource));
			
			ResourceList<Resource> list;
			list.m_resources = reinterpret_cast<Resource*>(mem);
			list.m_count = sizeof...(resources);
			return list;
		}

		const UniformInfo& getInfo(UniformHandle handle);

		/*template<class Info, class... Infos>
		void fill(Info info, UniformList& list, size_t index)
		{
			list.m_resources[index] = &getInfo(handle);
		}

		template<class Info, class... Infos>
		void fill(Info info, Infos infos, UniformList& list, size_t index)
		{

		}*/

		template<class... Infos>
		UniformList createUniformList(Infos... resources)
		{
			UniformList list = createResourceList<const UniformInfo*>(resources...);

			UniformHandle handles[] = { resources... };

			size_t i = 0;
			for (UniformHandle handle : handles)
			{
				//const UniformInfo* ptr = ;
				list.m_resources[i] = &(getInfo(handle));
				++i;
			}

			return list;
		}

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