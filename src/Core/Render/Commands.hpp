#pragma once

#include "RenderDefinitions.hpp"

namespace Phoenix
{
	// A Command is a struct that can be interpreted using a RenderContext
	// in order to perform a certain action using the GPU. The command can
	// contain any data required for it to be executed. A command must 
	// contain a static pointer to a funtion with the SubmitFptr signature -
	// this is the submit function. The submit function uses the command's 
	// data and the RenderContext to execute the action.
	namespace Commands
	{
		struct DrawIndexed
		{
			SUBMITTABLE();

			uint32_t start;
			uint32_t count;

			Primitive::Type primitives;
			VertexBufferHandle vertexBuffer;
			IndexBufferHandle indexBuffer;
			StateGroup state;
		};

		struct DrawLinear
		{
			SUBMITTABLE();

			uint32_t start;
			uint32_t count;

			Primitive::Type primitives;
			VertexBufferHandle vertexBuffer;
			StateGroup state;
		};

		struct SetUniform
		{
			SUBMITTABLE();

			UniformHandle handle;
			const void* data;
		};
	}
}