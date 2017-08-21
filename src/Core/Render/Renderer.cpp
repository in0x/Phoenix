#include "Renderer.hpp"
#include "IRenderContext.hpp"

namespace Phoenix
{
	namespace RenderSubmit
	{
		void IndexedDraw(IRenderContext* rc, const void* command)
		{
			const Commands::DrawIndexed* dc = static_cast<const Commands::DrawIndexed*>(command);

			rc->setVertexBuffer(dc->vertexBuffer);
			rc->setIndexBuffer(dc->indexBuffer);
			rc->drawIndexed(dc->primitives, dc->count, dc->start);
		}

		void LinearDraw(IRenderContext* rc, const void* command)
		{
			const Commands::DrawLinear* dc = static_cast<const Commands::DrawLinear*>(command);

			rc->setVertexBuffer(dc->vertexBuffer);
			rc->drawLinear(dc->primitives, dc->count, dc->start);
		}

		void TestDraw(IRenderContext* rc, const void* command)
		{
			rc->getMaxTextureUnits();
		}
	}

	const SubmitFunc Commands::DrawIndexed::SUBMIT_FUNC = RenderSubmit::IndexedDraw;
	const SubmitFunc Commands::DrawLinear::SUBMIT_FUNC = RenderSubmit::LinearDraw;

	void Renderer::submit(IRenderContext* rc)
	{
		for (size_t i = 0; i < m_currentIndex; ++i)
		{
			void* command = m_commands[i];
			SubmitFunc* submitFunc = Commands::loadSubmitFunc(command);
			(*submitFunc)(rc, command);

			//free(command);
		}

		m_currentIndex = 0;
		rc->swapBuffer();
	}
}