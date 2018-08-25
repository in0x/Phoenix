#pragma once

#include <Render/RIResourceHandles.hpp>

#include <Math/Vec4.hpp>
#include <Math/Vec3.hpp>
#include <Math/Vec2.hpp>

#include <vector>

namespace Phoenix
{	
	class IRIDevice;
	class IRIContext;
	class AssetRegistry;
	struct Material;
	struct LoadResources;

	struct MeshData
	{
		MeshData()
			: m_numVertices(0)
		{}

		void setSize(size_t numVertices);

		size_t m_numVertices;

		std::vector<Vec3> m_vertices;
		
		std::vector<Vec3> m_normals;
		
		// Surface-local coordinate handedness is stored in w.
		// Allows to calculate bitangent as B = (N x T) * Tw.
		std::vector<Vec4> m_tangents;

		std::vector<Vec2> m_texCoords;
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
		
		Material* m_materials[MAX_MATERIALS]; 
		size_t m_vertexFrom[MAX_MATERIALS]; 
		uint8_t m_numMaterials;
	};

	void createMeshBuffers(StaticMesh* outMesh, IRIDevice* renderDevice);

	StaticMesh* loadStaticMesh(const char* path, LoadResources* resources);

	void saveStaticMesh(StaticMesh& mesh, AssetRegistry* assets);
}