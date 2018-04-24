#pragma once

#include <Render/RIResourceHandles.hpp>

#include <Math/Vec3.hpp>
#include <Math/Vec2.hpp>

#include <Core/Texture.hpp>
#include <vector>
#include <memory>

namespace Phoenix
{	
	class IRIDevice;
	class IRIContext;

	struct Material
	{
		std::string m_name;

		Texture2D m_diffuseTex; 
		Texture2D m_roughnessTex;
		Texture2D m_metallicTex;
		Texture2D m_normalTex;
	};

	struct MeshData
	{
		MeshData()
			: m_numVertices(0)
		{}

		size_t m_numVertices;

		std::vector<Vec3> m_vertices;
		std::vector<Vec3> m_normals;
		std::vector<Vec2> m_uvs;
	};

	struct StaticMesh
	{
		StaticMesh()
			: m_vertexbuffer()
		{}

		enum
		{
			MAX_MATERIALS = 8
		};

		std::string m_name;

		MeshData m_data;
		VertexBufferHandle m_vertexbuffer;	
		
		Material m_materials[MAX_MATERIALS]; 
		size_t m_vertexFrom[MAX_MATERIALS]; 
		uint8_t m_numMaterials;
	};

	// Imports the specified mesh and the associated materials.
	std::vector<StaticMesh> importObj(const char* path, IRIDevice* renderDevice, IRIContext* renderContext);

	// Load a StaticMesh from disk.
	StaticMesh loadStaticMesh(const char* path, IRIDevice* renderDevice, IRIContext* renderContext);

	struct Archive;
	void serialize(Archive& ar, StaticMesh& mesh);
	void serialize(Archive& ar, Material& material);
}