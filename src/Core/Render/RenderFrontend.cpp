#include "RenderFrontend.hpp"

#include "WGlRenderBackend.hpp"
#include "UniformStore.hpp"
#include "CommandBucket.hpp"
#include "Commands.hpp"

#include "../Memory/StackAllocator.hpp"
#include "../Math/PhiMath.hpp"

#include <memory>
#include <assert.h>

namespace Phoenix
{
	namespace RenderFrontend
	{
		struct Members
		{
			Members(RenderInit* renderInit)
				: store(1024) // Temp
				, resourceListMemory(renderInit->m_resourceListMemoryBytes)
			{
				Logger::log("RenderFrontend uniform store still uses a fixed size, this needs to be fixed");

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

				uint32_t maxUniforms = renderBackend->getMaxUniformCount();
				renderBackend->getMaxTextureUnits();
			}

			CommandBucket bucket{ 1024, 4096 };
			UniformStore store;
			StackAllocator resourceListMemory;
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

		IRenderBackend* getBackend()
		{
			return s->renderBackend.get();
		}

		void* allocResourceList(size_t size, size_t alignment)
		{
			return s->resourceListMemory.allocate(size, alignment);
		}

		const UniformInfo& getInfo(UniformHandle handle)
		{
			return s->store.get(handle);
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

		UniformHandle createUniform(const char* name, Uniform::Type type, const void* data, size_t dataSize)
		{
			UniformHandle handle = createUniformHandle();
			handle.idx = s->uniforms++;

			s->renderBackend->createUniform(handle, name, type); // NOTE(Phil): This is useless for now.
			s->store.create(handle, type, name, data, dataSize);

			if (handle.isValid() && data != nullptr)
			{
				setUniform(handle, data, dataSize);
			}

			return handle;
		}

		void setUniform(UniformHandle handle, const void* data, size_t dataSize)
		{
			if (!handle.isValid())
			{
				Logger::error("Attempted to set value of invalid uniform");
				return;
			}
			
			s->store.update(handle, data, dataSize);
		}
		
		void drawIndexed(VertexBufferHandle vb, IndexBufferHandle ib, Primitive::Type primitives, uint32_t start, uint32_t count, StateGroup& state)
		{
			auto dc = s->bucket.addCommand<Commands::DrawIndexed>();

			dc->vertexBuffer = vb;
			dc->indexBuffer = ib;
			dc->primitives = Primitive::Triangles;
			dc->start = 0;
			dc->count = count;

			dc->state = state;
		}

		void clearFrameBuffer(FrameBufferHandle frame, Buffer::Type bitToClear, RGBA color)
		{
			auto dc = s->bucket.addCommand<Commands::ClearBuffer>();

			dc->handle = frame;
			dc->toClear = bitToClear;
			dc->color = color;
		}
	};
}