#pragma once

#include <Render/RIResourceHandles.hpp>
#include <vector>

namespace Phoenix
{	
	class IRIDevice;
	
	struct StaticMesh
	{
		StaticMesh()
			: m_vertexbuffer()
			, m_indexbuffer()
			, m_numVertices(0)
			, m_numIndices(0)
		{}

		VertexBufferHandle m_vertexbuffer;
		IndexBufferHandle m_indexbuffer;
		size_t m_numVertices;
		size_t m_numIndices;
	};

	std::vector<StaticMesh> loadRenderMesh(const char* path, IRIDevice* renderDevice);
}