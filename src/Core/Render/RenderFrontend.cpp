#include "RenderFrontend.hpp"

#include "WGlRenderBackend.hpp"
#include "CommandBucket.hpp"
#include "UniformCache.hpp"
#include "Commands.hpp"

#include <memory>

namespace Phoenix
{
	namespace RenderFrontend
	{
		static UniformCache s_uniformCache;
		static CommandBucket s_bucket = CommandBucket(1024);
		static std::unique_ptr<IRenderBackend> s_renderBackend;

		uint32_t m_vertexBuffers = 0;
		uint32_t m_indexBuffers = 0;
		uint32_t m_shaders = 0;
		uint32_t m_programs = 0;

		void init(RenderInit* renderInit)
		{
			switch (renderInit->getApiType())
			{
				case RenderApi::Gl:
				{
					s_renderBackend = std::make_unique<WGlRenderBackend>();
					s_renderBackend->init(renderInit);
					break;
				}
				case RenderApi::None:
				default:
				{
					Logger::error("Invalid RenderApi type");
					break;
				}
			}
		}

		void submit()
		{
			s_bucket.submit(s_renderBackend.get());
		}

		void swapBuffers()
		{
			s_renderBackend->swapBuffer();
		}

		void exit()
		{
			s_renderBackend = nullptr;
		}

		void tempSetProgram(ProgramHandle handle)
		{
			s_renderBackend->setProgram(handle);
		}

		IRenderBackend* getBackend()
		{
			return s_renderBackend.get();
		}

		VertexBufferHandle createVertexBuffer(const VertexBufferFormat& format)
		{
			VertexBufferHandle handle;
			handle.idx = ++m_vertexBuffers;

			auto cvb = s_bucket.addCommand<Commands::CreateVertexBuffer>();
			cvb->format = format;
			cvb->handle = handle;

			return handle;
		}

		IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data)
		{
			IndexBufferHandle handle;
			handle.idx = ++m_indexBuffers;

			auto cib = s_bucket.addCommand<Commands::CreateIndexBuffer>();
			cib->size = size;
			cib->count = count;
			cib->data = data;
			cib->handle = handle;

			return handle;
		}

		ShaderHandle createShader(const char* source, size_t strlen, Shader::Type shaderType)
		{
			ShaderHandle handle;
			handle.idx = ++m_shaders;

			Commands::CreateShader* cs = s_bucket.addCommand<Commands::CreateShader>(strlen);
			cs->shaderType = shaderType;
			cs->handle = handle;
			cs->source = commandPacket::getAuxiliaryMemory(cs);
			memcpy(commandPacket::getAuxiliaryMemory(cs), source, strlen);
			cs->source[strlen] = '\0';

			return handle;
		}

		ProgramHandle createProgram(const Shader::List& shaders)
		{
			ProgramHandle handle;
			handle.idx = ++m_programs;

			auto createProg = s_bucket.addCommand<Commands::CreateProgram>();
			
			createProg->shaders = shaders;
			createProg->handle = handle;

			return handle;
		}

	};
}