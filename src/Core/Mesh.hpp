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
		//Texture2DHandle m_roughnessTex;
		//Texture2DHandle m_metallicTex;
		//Texture2DHandle m_normalTex;
	};

	struct MeshData
	{
		MeshData()
			: m_numVertices(0)
			, m_vertices(nullptr)
			, m_normals(nullptr)
			, m_uvs(nullptr)
		{}
		
		// TODO: This really needs to replaced with a move once asset ownership is sorted out.
		MeshData(const MeshData& other)
		{
			m_numVertices = other.m_numVertices;

			m_vertices = new Vec3[m_numVertices];
			memcpy(m_vertices, other.m_vertices, m_numVertices * sizeof(Vec3));

			m_normals = new Vec3[m_numVertices];
			memcpy(m_normals, other.m_normals, m_numVertices * sizeof(Vec3));

			m_uvs = new Vec2[m_numVertices];
			memcpy(m_uvs, other.m_uvs, m_numVertices * sizeof(Vec2));
		}

		MeshData& operator=(const MeshData& other)
		{
			delete[] m_vertices;
			delete[] m_normals;
			delete[] m_uvs;

			m_numVertices = other.m_numVertices;

			m_vertices = new Vec3[m_numVertices];
			memcpy(m_vertices, other.m_vertices, m_numVertices * sizeof(Vec3));

			m_normals = new Vec3[m_numVertices];
			memcpy(m_normals, other.m_normals, m_numVertices * sizeof(Vec3));

			m_uvs = new Vec2[m_numVertices];
			memcpy(m_uvs, other.m_uvs, m_numVertices * sizeof(Vec2));
		}

		~MeshData()
		{
			delete[] m_vertices;
			delete[] m_normals;
			delete[] m_uvs;
		}

		size_t m_numVertices;

		Vec3* m_vertices;
		Vec3* m_normals;
		Vec2* m_uvs;
	};

	/*std::unique_ptr<Vec3> m_vertices;
	std::unique_ptr<Vec3> m_normals;
	std::unique_ptr<Vec2> m_uvs;*/

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
	std::vector<StaticMesh> loadStaticMesh(const char* path, IRIDevice* renderDevice, IRIContext* renderContext);

	struct Archive;
	void serialize(Archive& ar, StaticMesh& mesh);
	void serialize(Archive& ar, Material& material);
}