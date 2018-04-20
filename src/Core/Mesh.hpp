#pragma once

#include <Render/RIResourceHandles.hpp>
#include <Math/Vec3.hpp>
#include <vector>

namespace Phoenix
{	
	class IRIDevice;
	class IRIContext;

	struct Material
	{
		Texture2DHandle m_diffuseTex;
		Texture2DHandle m_roughnessTex;
		Texture2DHandle m_metallicTex;
		Texture2DHandle m_normalTex;
	};

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

		enum
		{
			MAX_MATERIALS = 8
		};

		Material m_materials[MAX_MATERIALS]; // TODO: Replace with references.
		size_t m_vertexFrom[MAX_MATERIALS]; // From what vertex the materials applies. Enables multiple materials.
		uint8_t m_numMaterials;
	};

	// Imports the specified mesh and the associated materials.
	std::vector<StaticMesh> loadStaticMesh(const char* path, IRIDevice* renderDevice, IRIContext* renderContext);
}