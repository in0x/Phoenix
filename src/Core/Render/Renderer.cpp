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

		void indexBufferCreate(IRenderContext* rc, const void* command)
		{
			auto dc = static_cast<const Commands::CreateIndexBuffer*>(command);
			rc->createIndexBuffer(dc->handle, dc->size, dc->count, dc->data);
		}

		void shaderCreate(IRenderContext* rc, const void* command)
		{
			auto dc = static_cast<const Commands::CreateShader*>(command);
			rc->createShader(dc->handle, dc->source, dc->shaderType);
		}

		void programCreate(IRenderContext* rc, const void* command)
		{
			auto dc = static_cast<const Commands::CreateProgram*>(command);
			rc->createProgram(dc->handle, dc->shaders);
		}

		void uniformCreate(IRenderContext* rc, const void* command)
		{
			auto dc = static_cast<const Commands::CreateUniform*>(command);

			//Uniform::Type dataType;
			//const void* data;
		}
	}

	const SubmitFptr Commands::DrawIndexed::SubmitFunc = SubmitFunctions::indexedDraw;
	const SubmitFptr Commands::DrawLinear::SubmitFunc = SubmitFunctions::linearDraw;
	const SubmitFptr Commands::CreateVertexBuffer::SubmitFunc = SubmitFunctions::vertexBufferCreate;
	const SubmitFptr Commands::CreateIndexBuffer::SubmitFunc = SubmitFunctions::indexBufferCreate;
	const SubmitFptr Commands::CreateShader::SubmitFunc = SubmitFunctions::shaderCreate;
	const SubmitFptr Commands::CreateProgram::SubmitFunc = SubmitFunctions::programCreate;

}