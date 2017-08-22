#include "Renderer.hpp"
#include "IRenderContext.hpp"

namespace Phoenix
{
	namespace commandPacket
	{		
		CommandPacket* getNextCommandPacket(CommandPacket packet)
		{
			return reinterpret_cast<CommandPacket*>(reinterpret_cast<char*>(packet) + OFFSET_NEXT_COMMAND_PACKET);
		}

		SubmitFptr* getSubmitFptr(CommandPacket packet)
		{
			return reinterpret_cast<SubmitFptr*>(reinterpret_cast<char*>(packet) + OFFSET_BACKEND_DISPATCH_FUNCTION);
		}

		void setNextCommandPacket(CommandPacket packet, CommandPacket nextPacket)
		{
			*commandPacket::getNextCommandPacket(packet) = nextPacket;
		}

		void setSubmitFptr(CommandPacket packet, SubmitFptr dispatchFunction)
		{
			*commandPacket::getSubmitFptr(packet) = dispatchFunction;
		}

		const CommandPacket loadNextCommandPacket(const CommandPacket packet)
		{
			return *getNextCommandPacket(packet);
		}

		const SubmitFptr loadSubmitFptr(const CommandPacket packet)
		{
			return *getSubmitFptr(packet);
		}

		const void* loadCommand(const CommandPacket packet)
		{
			return reinterpret_cast<char*>(packet) + OFFSET_COMMAND;
		}
	};

	namespace Commands
	{
		// When enqueuing command, a handle is returned upfront from the context, 
		// into which is written when the command is submitted later.
		struct CreateVertexBuffer
		{
			SUBMITTABLE();

			VertexBufferFormat format;
			VertexBufferHandle handle;
		};

		struct CreateIndexBuffer
		{
			const static SubmitFptr SubmitFunc;

			size_t size;
			uint32_t count;
			const void* data;
		};

		struct CreateShader
		{
			const static SubmitFptr SubmitFunc;

			const char* source;
			Shader::Type shaderType;
		};

		struct CreateProgram
		{
			const static SubmitFptr SubmitFunc;

			Shader::List shaders;
		};
	}

	namespace SubmitFunctions
	{
		void indexedDraw(IRenderContext* rc, const void* command)
		{
			auto dc = static_cast<const Commands::DrawIndexed*>(command);

			rc->setVertexBuffer(dc->vertexBuffer);
			rc->setIndexBuffer(dc->indexBuffer);
			rc->drawIndexed(dc->primitives, dc->count, dc->start);
		}

		void linearDraw(IRenderContext* rc, const void* command)
		{
			auto dc = static_cast<const Commands::DrawLinear*>(command);

			rc->setVertexBuffer(dc->vertexBuffer);
			rc->drawLinear(dc->primitives, dc->count, dc->start);
		}

		void vertexBufferCreate(IRenderContext* rc, const void* command)
		{
			auto dc = static_cast<const Commands::CreateVertexBuffer*>(command);
			rc->createVertexBuffer(dc->handle, dc->format);
		}
	}

	const SubmitFptr Commands::DrawIndexed::SubmitFunc = SubmitFunctions::indexedDraw;
	const SubmitFptr Commands::DrawLinear::SubmitFunc = SubmitFunctions::linearDraw;
	const SubmitFptr Commands::CreateVertexBuffer::SubmitFunc = SubmitFunctions::vertexBufferCreate;

	VertexBufferHandle Renderer::createVertexBuffer(const VertexBufferFormat& vbf)
	{
		VertexBufferHandle handle = m_pContext->allocVertexBuffer();
		
		if (handle.isValid())
		{
			auto cmd = addCommand<Commands::CreateVertexBuffer>();
			cmd->format = vbf;
			cmd->handle = handle;
		}
		else
		{
			Logger::error("Failed to allocate VertexBuffer");
		}

		return handle;
	}

	// Need to implement commands and submitfunc for these resources.
	ShaderHandle Renderer::createShader(const char* source, Shader::Type shaderType)
	{

	}
	
	ProgramHandle Renderer::createProgram(const Shader::List& shaders)
	{

	}
	
	TextureHandle Renderer::createTexture()
	{
		return m_pContext->allocTexture();
	}
	
	FrameBufferHandle Renderer::createFrameBuffer()
	{
		return m_pContext->allocFrameBuffer();
	}
	
	UniformHandle Renderer::createUniform(const char* name, Uniform::Type type)
	{
		return m_pContext->allocUniform();
	}
}