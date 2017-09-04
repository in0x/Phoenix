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
		//static UniformCache s_uniformCache;
		static std::unique_ptr<IRenderBackend> s_renderBackend = nullptr;

		static uint32_t s_vertexBuffers = 0;
		static uint32_t s_indexBuffers = 0;
		static uint32_t s_shaders = 0;
		static uint32_t s_programs = 0;

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
			handle.idx = s_vertexBuffers++;

			s_renderBackend->createVertexBuffer(handle, format);

			return handle;
		}

		IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data)
		{
			IndexBufferHandle handle;
			handle.idx = s_indexBuffers++;

			s_renderBackend->createIndexBuffer(handle, size, count, data);

			return handle;
		}

		ShaderHandle createShader(const char* source, size_t strlen, Shader::Type shaderType)
		{
			ShaderHandle handle;
			handle.idx = s_shaders++;

			s_renderBackend->createShader(handle, source, shaderType);
			
			return handle;
		}

		ProgramHandle createProgram(const Shader::List& shaders)
		{
			ProgramHandle handle;
			handle.idx = s_programs++;

			s_renderBackend->createProgram(handle, shaders);
		
			return handle;
		}

		UniformHandle createUniform(ProgramHandle program, const char* name, Uniform::Type type, const void* data)
		{
			UniformHandle handle;
			s_renderBackend->createUniform(program, handle, name, type, nullptr); // TODO(Phil): Need to fix data ptr being passed here.
			return handle;
		}
	};
}