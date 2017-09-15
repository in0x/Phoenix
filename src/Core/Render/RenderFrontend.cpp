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

			CommandBucket bucket{ 1024, 4096 };
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

		VertexBufferHandle createVertexBuffer(const VertexBufferFormat& format)
		{
			VertexBufferHandle handle;
			handle.idx = s->vertexBuffers++;

			s->renderBackend->createVertexBuffer(handle, format);

			return handle;
		}

		IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data)
		{
			IndexBufferHandle handle;
			handle.idx = s->indexBuffers++;

			s->renderBackend->createIndexBuffer(handle, size, count, data);

			return handle;
		}

		ShaderHandle createShader(const char* source, Shader::Type shaderType)
		{
			ShaderHandle handle;
			handle.idx = s->shaders++;

			s->renderBackend->createShader(handle, source, shaderType);

			return handle;
		}

		ProgramHandle createProgram(const Shader::List& shaders)
		{
			ProgramHandle handle;
			handle.idx = s->programs++;

			s->renderBackend->createProgram(handle, shaders);

			return handle;
		}

		UniformHandle createUniform(ProgramHandle program, const char* name, Uniform::Type type, const void* data, size_t dataSize)
		{
			UniformHandle handle;
			handle.idx = s->uniforms++;

			s->renderBackend->createUniform(program, handle, name, type);

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
			
			auto set = s->bucket.addCommand<Commands::SetUniform>(dataSize);

			set->handle = handle;
			set->data = commandPacket::getAuxiliaryMemory(set);
			memcpy(commandPacket::getAuxiliaryMemory(set), data, dataSize);
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