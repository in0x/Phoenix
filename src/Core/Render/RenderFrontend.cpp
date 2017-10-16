#include "RenderFrontend.hpp"

#include "WGlRenderBackend.hpp"
#include "UniformStore.hpp"
#include "CommandBucket.hpp"
#include "Commands.hpp"

#include "../Logger.hpp"
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
			{
				Logger::log("RenderFrontend uniform store still uses a fixed size, this needs to be fixed");

				switch (renderInit->getApiType())
				{
				case ERenderApi::Gl:
				{
					renderBackend = std::make_unique<WGlRenderBackend>();
					renderBackend->init(renderInit);
					break;
				}
				case ERenderApi::None:
				default:
				{
					Logger::error("Invalid RenderApi type");
					break;
				}
				}

				uint32_t maxUniforms = renderBackend->getMaxUniformCount();
				renderBackend->getMaxTextureUnits();
			}

			CommandBucket bucket{ 1024, MEGABYTE(24) };
			UniformStore store;
			std::unique_ptr<IRenderBackend> renderBackend = nullptr;

			uint32_t vertexBuffers = 0;
			uint32_t indexBuffers = 0;
			uint32_t shaders = 0;
			uint32_t programs = 0;
			uint32_t uniforms = 0;
			uint32_t textures = 0;
		};

		static std::unique_ptr<Members> s = nullptr;

		void init(RenderInit* renderInit)
		{
			s = std::make_unique<Members>(renderInit);
			Logger::log(std::to_string(sizeof(Commands::DrawIndexed)).c_str());
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

		const UniformInfo& getInfo(size_t id)
		{
			return s->store.get(id);
		}

		VertexBufferHandle createVertexBuffer(const VertexBufferFormat& format)
		{
			VertexBufferHandle handle = createVertexBufferHandle(s->vertexBuffers++);
			
			auto cvb = s->bucket.addCommand<Commands::CreateVertexBuffer>();
			cvb->format = format;
			cvb->handle = handle;

			return handle;
		}

		IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data)
		{
			IndexBufferHandle handle = createIndexBufferHandle(s->indexBuffers++);
			
			auto cib = s->bucket.addCommand<Commands::CreateIndexBuffer>();
			cib->size = size;
			cib->count = count;
			cib->data = data;
			cib->handle = handle;

			return handle;
		}

		ShaderHandle createShader(const char* source, EShader::Type shaderType)
		{
			ShaderHandle handle = createShaderHandle(s->shaders++);
			size_t strLen = strlen(source);

			Commands::CreateShader* cs = s->bucket.addCommand<Commands::CreateShader>(strLen);
			cs->shaderType = shaderType;
			cs->handle = handle;
			cs->source = commandPacket::getAuxiliaryMemory(cs);
			memcpy(commandPacket::getAuxiliaryMemory(cs), source, strLen);
			cs->source[strLen] = '\0';

			return handle;
		}

		ProgramHandle createProgram(const EShader::List& shaders)
		{
			ProgramHandle handle = createProgramHandle(s->programs++);
			
			auto createProg = s->bucket.addCommand<Commands::CreateProgram>();

			createProg->shaders = shaders;
			createProg->handle = handle;

			return handle;
		}

		UniformHandle createUniform(const char* name, EUniform::Type type, const void* data, size_t dataSize)
		{
			UniformHandle handle = createUniformHandle(s->uniforms++);
			
			s->store.create(handle.idx, type, name, data, dataSize);

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
			
			s->store.update(handle.idx, data, dataSize);
		}

		size_t uniformMem(const StateGroup& state)
		{
			return (state.uniformCount + state.textureCount) * sizeof(UniformInfo);
		}

		size_t textureMem(const StateGroup& state)
		{
			return state.textureCount * sizeof(TextureHandle);
		}

		size_t resourceMem(const StateGroup& state)
		{
			size_t memory = uniformMem(state);
			memory += textureMem(state);
			return memory;
		}

		char* copyUniforms(char* memory, const StateGroup& state)
		{
			for (size_t i = 0; i < state.uniformCount; ++i)
			{
				const UniformInfo& info = getInfo(state.uniforms[i].idx);
				memcpy(memory, &info, sizeof(UniformInfo));
				memory += sizeof(UniformInfo);
			}

			return memory;
		}

		char* copyTextureLocations(char* memory, const StateGroup& state)
		{
			for (size_t i = 0; i < state.textureCount; ++i)
			{
				const UniformInfo& info = getInfo(state.textures[i].uniformHandle.idx);
				memcpy(memory, &info, sizeof(UniformInfo));
				memory += sizeof(UniformInfo);
			}

			return memory;
		}

		char* copyTextures(char* memory, const StateGroup& state)
		{
			for (size_t i = 0; i < state.textureCount; ++i)
			{
				memcpy(memory, &state.textures[i], sizeof(TextureHandle));
				memory += sizeof(TextureHandle);
			}

			return memory;
		}

		void copyState(CStateGroup& cmdState, void* memory, const StateGroup& state)
		{
			cmdState.blend = state.blend;
			cmdState.depth = state.depth;
			cmdState.program = state.program;
			cmdState.raster = state.raster;
			cmdState.stencil = state.stencil;
			cmdState.uniformCount = state.uniformCount;
			cmdState.textureCount = state.textureCount;

			cmdState.uniforms = static_cast<UniformInfo*>(memory); 
			char* writeLocation = reinterpret_cast<char*>(memory);
			writeLocation = copyUniforms(writeLocation, state);

			cmdState.textureLocations = reinterpret_cast<UniformInfo*>(writeLocation);
			writeLocation = copyTextureLocations(writeLocation, state);

			cmdState.textures = reinterpret_cast<TextureHandle*>(writeLocation);
			copyTextures(writeLocation, state);
		}

		TextureHandle createTexture(const ETexture::Description& desc, ETexture::Format format, const char* samplerName)
		{
			TextureHandle handle = createTextureHandle(s->textures++, s->uniforms++);
			size_t strLen = strlen(samplerName);

			auto ct = s->bucket.addCommand<Commands::createTexture>(strLen);
			ct->desc = desc;
			ct->handle = handle;
			ct->name = commandPacket::getAuxiliaryMemory(ct);
			ct->format = format;
			memcpy(commandPacket::getAuxiliaryMemory(ct), samplerName, strLen);
			ct->name[strLen] = '\0';

			s->store.create(handle.uniformHandle.idx, EUniform::Int, samplerName, 0, 0);

			return handle;
		}

		TextureHandle createTexture(const TextureDesc& desc, const void* data, ETexture::Format format, const char* samplerName)
		{
			TextureHandle handle = createTexture(desc, format, samplerName);
			
			auto ut = s->bucket.addCommand<Commands::UploadTexture>();
			ut->data = data;
			ut->handle = handle;
			ut->width = desc.width;
			ut->height = desc.height;

			return handle;
		}

		TextureHandle createCubemap(const TextureDesc& desc, const char* samplerName, const CubemapData& cubemap)
		{
			TextureHandle handle = createTexture(desc, ETexture::CubeMap, samplerName);

			auto ut = s->bucket.addCommand<Commands::UploadCubemap>();

			ut->data = cubemap;
			ut->handle = handle;
			ut->width = desc.width;
			ut->height = desc.height;

			return handle;
		}
		
		void drawIndexed(VertexBufferHandle vb, IndexBufferHandle ib, EPrimitive::Type primitives, uint32_t start, uint32_t count, StateGroup& state)
		{
			auto dc = s->bucket.addCommand<Commands::DrawIndexed>(resourceMem(state));

			dc->vertexBuffer = vb;
			dc->indexBuffer = ib;
			dc->primitives = EPrimitive::Triangles;
			dc->start = 0;
			dc->count = count;

			void* memory = commandPacket::getAuxiliaryMemory(dc);
			copyState(dc->state, memory, state);
		}

		void clearFrameBuffer(FrameBufferHandle frame, EBuffer::Type bitToClear, RGBA color)
		{
			auto dc = s->bucket.addCommand<Commands::ClearBuffer>();

			dc->handle = frame;
			dc->toClear = bitToClear;
			dc->color = color;
		}
	};
}