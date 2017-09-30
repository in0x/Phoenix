
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

		void* allocResource(size_t size, size_t alignment);

		void freeResource(void* memory);
	
		template <class Resource>
		void destroyResourceList(ResourceList<Resource>& list)
		{
			list.m_count = 0;
			freeResource(list.m_resources);
		}

		template<class Resource, class... Resources>
		ResourceList<Resource> createResourceList(Resources... resources)
		{
			void* mem = allocResource(sizeof...(resources) * sizeof(Resource), alignof(Resource));

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

		void drawIndexed(VertexBufferHandle vb, IndexBufferHandle ib, EPrimitive::Type primitives, uint32_t start, uint32_t count, StateGroup& state);

		void clearFrameBuffer(FrameBufferHandle frame, EBuffer::Type bitToClear, RGBA color);
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


// NOTE(Phil): Sketch to replace Frontend

//#include "CommandBucket.hpp"
//#include "Commands.hpp"
//
//namespace Phoenix
//{
//	// Open questions:
//	// * Where does UniformStore go? One per submitbucket?
//
//	CommandBucket createBucket();
//
//	// Utility creation functions that take bucket and insert command
//	VertexBufferHandle createVertexBuffer(CommandBucket& bucket, const VertexBufferFormat& format)
//	{
//		VertexBufferHandle handle = createVertexBufferHandle();
//		
//		auto cvb = bucket.addCommand<Commands::CreateVertexBuffer>();
//		cvb->format = format;
//		cvb->handle = handle;
//
//		return handle;
//	}
//
//	// Seperate buckets for allocation and submission, Machinery does this, look up again why,
//	// think it has to do with sorting.
//	class CreateBucket;
//	class SubmitBucket;
//	CreateBucket allocCreateBucket();
//	SubmitBucket allocSubmitBucket();
//}