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
	
		/*void uniformSet(IRenderBackend* rc, const void* command)
		{
			auto dc = static_cast<const Commands::SetUniform*>(command);
			rc->setUniform(dc->handle, dc->data);
		}*/

		void clearBuffer(IRenderBackend* rc, const void* command)
		{
			auto dc = static_cast<const Commands::ClearBuffer*>(command);
			rc->clearFrameBuffer(dc->handle, dc->toClear, dc->color);
		}
	}

	const SubmitFptr Commands::DrawIndexed::SubmitFunc = SubmitFunctions::indexedDraw;
	const SubmitFptr Commands::DrawLinear::SubmitFunc = SubmitFunctions::linearDraw;
	//const SubmitFptr Commands::SetUniform::SubmitFunc = SubmitFunctions::uniformSet;
	const SubmitFptr Commands::ClearBuffer::SubmitFunc = SubmitFunctions::clearBuffer;
}
