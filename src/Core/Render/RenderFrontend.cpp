#include "RenderFrontend.hpp"

#include "WGlRenderBackend.hpp"
#include "CommandBucket.hpp"
#include "UniformCache.hpp"
#include "Commands.hpp"

#include <memory>
#include <cassert>

namespace Phoenix
{
	namespace RenderFrontend
	{
		struct Members
		{
			Members(RenderInit* renderInit)
			{
				switch (renderInit->getApiType())
				{
				case RenderApi::Gl:
				{
					renderBackend = std::make_unique<WGlRenderBackend>();
					renderBackend->init(renderInit);
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

			CommandBucket bucket = CommandBucket(1024);
			std::unique_ptr<IRenderBackend> renderBackend = nullptr;

			uint32_t vertexBuffers = 0;
			uint32_t indexBuffers = 0;
			uint32_t shaders = 0;
			uint32_t programs = 0;
			uint32_t uniforms = 0;
		};

		static std::unique_ptr<Members> s = nullptr;

		void init(RenderInit* renderInit)
		{
			s = std::make_unique<Members>(renderInit);
		}

		void submitCommands()
		{
			s->bucket.submit(s->renderBackend.get());
		}

		void swapBuffers()
		{
			s->renderBackend->swapBuffer();
		}

		void exit()
		{
			s = nullptr;
		}

		void tempSetProgram(ProgramHandle handle)
		{
			s->renderBackend->setProgram(handle);
		}

		IRenderBackend* getBackend()
		{
			return s->renderBackend.get();
		}

		VertexBufferHandle createVertexBuffer(const VertexBufferFormat& format)
		{
			VertexBufferHandle handle;
			handle.idx = s->vertexBuffers++;

			auto cvb = s->bucket.addCommand<Commands::CreateVertexBuffer>();
			cvb->format = format;
			cvb->handle = handle;

			return handle;
		}

		IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data)
		{
			IndexBufferHandle handle;
			handle.idx = s->indexBuffers++;

			auto cib = s->bucket.addCommand<Commands::CreateIndexBuffer>();
			cib->size = size;
			cib->count = count;
			cib->data = data;
			cib->handle = handle;

			return handle;
		}

		ShaderHandle createShader(const char* source, Shader::Type shaderType)
		{
			ShaderHandle handle;
			handle.idx = s->shaders++;
			size_t strLen = strlen(source);

			Commands::CreateShader* cs = s->bucket.addCommand<Commands::CreateShader>(strLen);
			cs->shaderType = shaderType;
			cs->handle = handle;
			cs->source = commandPacket::getAuxiliaryMemory(cs);
			memcpy(commandPacket::getAuxiliaryMemory(cs), source, strLen);
			cs->source[strLen] = '\0';

			return handle;
		}

		ProgramHandle createProgram(const Shader::List& shaders)
		{
			ProgramHandle handle;
			handle.idx = s->programs++;

			auto createProg = s->bucket.addCommand<Commands::CreateProgram>();

			createProg->shaders = shaders;
			createProg->handle = handle;

			return handle;
		}

		UniformHandle createUniform(ProgramHandle program, const char* name, Uniform::Type type, const void* data, size_t dataSize)
		{
			UniformHandle handle;
			handle.idx = s->uniforms++;

			size_t nameLen = strlen(name);
			auto create = s->bucket.addCommand<Commands::CreateUniform>(nameLen + dataSize);

			create->program = program;
			create->handle = handle;
			create->dataType = type;

			create->name = commandPacket::getAuxiliaryMemory(create);
			memcpy(create->name, name, nameLen);
			create->name[nameLen] = '\0';

			if (data != nullptr)
			{
				setUniform(handle, data, dataSize);
			}

			return handle;
		}

		void setUniform(UniformHandle handle, const void* data, size_t dataSize)
		{
			auto set = s->bucket.addCommand<Commands::SetUniform>(dataSize);

			set->handle = handle;

			set->data = commandPacket::getAuxiliaryMemory(set);
			memcpy(commandPacket::getAuxiliaryMemory(set), data, dataSize);
		}
	};
}