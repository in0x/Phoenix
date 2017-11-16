#pragma once

#include <Render/RIResourceHandles.hpp>

namespace Phoenix
{	
	class IRIDevice;

	struct RenderMesh
	{
		VertexBufferHandle m_vertexbuffer;
		IndexBufferHandle m_indexbuffer;
		size_t m_numVertices;
		size_t m_numIndices;
	};

	RenderMesh loadRenderMesh(const char* path, IRIDevice* renderDevice);
}