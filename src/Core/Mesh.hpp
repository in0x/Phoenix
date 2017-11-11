#pragma once

#include <Render/RIResourceHandles.hpp>

namespace Phoenix
{	
	class IRIDevice;

	struct RenderMesh
	{
		VertexBufferHandle vb;
		IndexBufferHandle ib;
		size_t numVertices;
		size_t numIndices;
	};

	RenderMesh loadRenderMesh(const char* path, IRIDevice* renderDevice);
}