#include "Mesh.hpp"

#include <assert.h>

#include <Core/Serialize.hpp>
#include <Core/SerialUtil.hpp>
#include <Core/Material.hpp>
#include <Core/AssetRegistry.hpp>

#include <Render/RIDevice.hpp>

namespace Phoenix
{
	void MeshData::setSize(size_t numVertices)
	{
		m_numVertices = numVertices;
		m_vertices.reserve(numVertices);
		m_normals.reserve(numVertices);
		m_tangents.reserve(numVertices);
		m_texCoords.reserve(numVertices);
	}
	
	void createMeshBuffers(StaticMesh* outMesh, IRIDevice* renderDevice)
	{
		size_t numVertices = outMesh->m_data.m_numVertices;

		VertexBufferFormat layout;
		layout.add({ EAttributeProperty::Position, EAttributeType::Float, 3 },
		{ sizeof(Vec3), numVertices, outMesh->m_data.m_vertices.data() });

		layout.add({ EAttributeProperty::Normal, EAttributeType::Float, 3 },
		{ sizeof(Vec3), numVertices, outMesh->m_data.m_normals.data() });

		layout.add({ EAttributeProperty::TexCoord, EAttributeType::Float, 2 },
		{ sizeof(Vec2), numVertices, outMesh->m_data.m_texCoords.data() });

		layout.add({ EAttributeProperty::Bitangent, EAttributeType::Float, 4 },
		{ sizeof(Vec4), numVertices, outMesh->m_data.m_tangents.data() });

		outMesh->m_vertexbuffer = renderDevice->createVertexBuffer(layout);

		assert(outMesh->m_vertexbuffer.isValid());
	}

	void serialize(Archive* ar, MeshData& data)
	{
		serialize(ar, data.m_numVertices);
		serialize(ar, data.m_vertices);
		serialize(ar, data.m_normals);
		serialize(ar, data.m_tangents);
		serialize(ar, data.m_texCoords);
	}

	struct MeshMaterialExport
	{
		MeshMaterialExport() = default;

		MeshMaterialExport(const StaticMesh& mesh)
		{
			for (uint8_t i = 0; i < mesh.m_numMaterials; ++i)
			{
				m_materialRefs[i] = mesh.m_materials[i]->m_name;
				m_matVertexFrom[i] = mesh.m_vertexFrom[i];
			}

			m_numMaterials = mesh.m_numMaterials;
		}

		std::string m_materialRefs[StaticMesh::MAX_MATERIALS];
		size_t m_matVertexFrom[StaticMesh::MAX_MATERIALS];

		uint8_t m_numMaterials;
	};

	void serialize(Archive* ar, MeshMaterialExport& exp)
	{
		serialize(ar, exp.m_numMaterials);

		for (uint8_t i = 0; i < exp.m_numMaterials; ++i)
		{
			serialize(ar, exp.m_materialRefs[i]);
			serialize(ar, exp.m_matVertexFrom[i]);
		}
	}

	void serialize(Archive* ar, StaticMesh& mesh)
	{
		serialize(ar, mesh.m_data);
		serialize(ar, mesh.m_name);
		serialize(ar, mesh.m_numMaterials);
	}

	static const char* g_assetFileExt = ".sm";

	void saveStaticMesh(StaticMesh& mesh, AssetRegistry* assets)
	{
		WriteArchive ar;
		createWriteArchive(sizeof(StaticMesh), &ar);

		serialize(&ar, mesh);

		for (uint8_t i = 0; i < mesh.m_numMaterials; ++i)
		{
			MeshMaterialExport exp(mesh);
			serialize(&ar, exp);
		}
		
		std::string writePath = assets->getAssetsPath() + mesh.m_name;
		writePath += g_assetFileExt;

		EArchiveError err = writeArchiveToDisk(writePath.c_str(), ar);
		assert(err == EArchiveError::NoError);
		destroyArchive(ar);
	}

	StaticMesh* loadStaticMesh(const char* path, IRIDevice* renderDevice, IRIContext* renderContext, AssetRegistry* assets)
	{	
		StaticMesh* mesh = assets->getStaticMesh(path);

		if (mesh)
		{
			return mesh;
		}

		std::string readPath = path;
		readPath = assets->getAssetsPath() + readPath + g_assetFileExt;

		ReadArchive ar;
		EArchiveError err = createReadArchive(readPath.c_str(), &ar);

		assert(err == EArchiveError::NoError);
		if (err != EArchiveError::NoError)
		{
			return mesh;
		}

		mesh = assets->allocStaticMesh(path);

		serialize(&ar, *mesh);
		createMeshBuffers(mesh, renderDevice);

		for (uint8_t i = 0; i < mesh->m_numMaterials; ++i)
		{
			MeshMaterialExport exp;
			serialize(&ar, exp);

			mesh->m_materials[i] = loadMaterial(exp.m_materialRefs[i].c_str(), renderDevice, renderContext, assets);
			mesh->m_vertexFrom[i] = exp.m_matVertexFrom[i];
		}

		destroyArchive(ar);

		return mesh;
	}
}