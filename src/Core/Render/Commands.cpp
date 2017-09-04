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
			rc->setProgram(dc->state.program);
			rc->drawIndexed(dc->primitives, dc->count, dc->start);
		}

		void linearDraw(IRenderBackend* rc, const void* command)
		{
			auto dc = static_cast<const Commands::DrawLinear*>(command);

			rc->setVertexBuffer(dc->vertexBuffer);
			rc->setProgram(dc->state.program);
			rc->drawLinear(dc->primitives, dc->count, dc->start);
		}
	}

	const SubmitFptr Commands::DrawIndexed::SubmitFunc = SubmitFunctions::indexedDraw;
	const SubmitFptr Commands::DrawLinear::SubmitFunc = SubmitFunctions::linearDraw;
}