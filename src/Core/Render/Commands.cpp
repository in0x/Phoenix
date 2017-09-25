#pragma once

#include "Commands.hpp"
#include "IRenderBackend.hpp"

namespace Phoenix
{
	namespace SubmitFunctions
	{
		void indexedDraw(IRenderBackend* rc, const void* command)
		{
			auto dc = static_cast<const Commands::DrawIndexed*>(command);

			rc->setVertexBuffer(dc->vertexBuffer);
			rc->setIndexBuffer(dc->indexBuffer);
			rc->setState(dc->state);
			rc->drawIndexed(dc->primitives, dc->count, dc->start);
		}

		void linearDraw(IRenderBackend* rc, const void* command)
		{
			auto dc = static_cast<const Commands::DrawLinear*>(command);

			rc->setVertexBuffer(dc->vertexBuffer);
			rc->setState(dc->state);
			rc->drawLinear(dc->primitives, dc->count, dc->start);
		}
	
		void clearBuffer(IRenderBackend* rc, const void* command)
		{
			auto dc = static_cast<const Commands::ClearBuffer*>(command);
			rc->clearFrameBuffer(dc->handle, dc->toClear, dc->color);
		}

		void vertexBufferCreate(IRenderBackend* rc, const void* command)
		{
			auto dc = static_cast<const Commands::CreateVertexBuffer*>(command);
			rc->createVertexBuffer(dc->handle, dc->format);
		}

		void indexBufferCreate(IRenderBackend* rc, const void* command)
		{
			auto dc = static_cast<const Commands::CreateIndexBuffer*>(command);
			rc->createIndexBuffer(dc->handle, dc->size, dc->count, dc->data);
		}

		void shaderCreate(IRenderBackend* rc, const void* command)
		{
			auto dc = static_cast<const Commands::CreateShader*>(command);
			rc->createShader(dc->handle, dc->source, dc->shaderType);
		}

		void programCreate(IRenderBackend* rc, const void* command)
		{
			auto dc = static_cast<const Commands::CreateProgram*>(command);
			rc->createProgram(dc->handle, dc->shaders);
		}
	}

	const SubmitFptr Commands::DrawIndexed::SubmitFunc = SubmitFunctions::indexedDraw;
	const SubmitFptr Commands::DrawLinear::SubmitFunc = SubmitFunctions::linearDraw;
	const SubmitFptr Commands::ClearBuffer::SubmitFunc = SubmitFunctions::clearBuffer;
	const SubmitFptr Commands::CreateVertexBuffer::SubmitFunc = SubmitFunctions::vertexBufferCreate;
	const SubmitFptr Commands::CreateIndexBuffer::SubmitFunc = SubmitFunctions::indexBufferCreate;
	const SubmitFptr Commands::CreateShader::SubmitFunc = SubmitFunctions::shaderCreate;
	const SubmitFptr Commands::CreateProgram::SubmitFunc = SubmitFunctions::programCreate;
}
