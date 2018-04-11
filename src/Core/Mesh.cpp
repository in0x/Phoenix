#include "Mesh.hpp"

#include <Core/Logger.hpp>
#include <Render/RIDevice.hpp>

#include <assert.h>
#include <memory>

#define IMPORT_OBJ_PHI 0
#define IMPORT_OBJ_TINYOBJ 1

#if IMPORT_OBJ_PHI
#include <Core/obj.hpp>
#endif

#if IMPORT_OBJ_TINYOBJ
#define TINYOBJLOADER_IMPLEMENTATION
#include <ThirdParty/tinyobj/tiny_obj_loader.h>
#include <Math/Vec3.hpp>
#include <Math/Vec2.hpp>
#endif

namespace Phoenix
{
#if IMPORT_OBJ_PHI

	StaticMesh loadRenderMesh(const char* path, IRIDevice* renderDevice)
	{
		StaticMesh mesh;

		const char* fileDot = strrchr(path, '.');
		size_t pathLen = strlen(path);
		if (nullptr == fileDot)
		{
			assert(false);
			Logger::errorf("Could not locate file ending in path %s while loading RenderMesh", path);
			return mesh;
		}

		if (strcmp(".obj", fileDot) == 0)
		{
			const char* lastSlash = strrchr(path, '/');
			lastSlash++;

			std::string pathToAsset(path, lastSlash);
			std::string assetName(lastSlash, path + pathLen);

			OBJImport import = loadObj(pathToAsset, assetName);

			mesh.m_numVertices = import.mesh.vertices.size();
			mesh.m_numIndices = import.mesh.indices.size();

			VertexBufferFormat layout;
			layout.add({ EAttributeProperty::Position, EAttributeType::Float, 3 },
			{ sizeof(Vec3), import.mesh.vertices.size(), import.mesh.vertices.data() });

			layout.add({ EAttributeProperty::Normal, EAttributeType::Float, 3 },
			{ sizeof(Vec3), import.mesh.normals.size(), import.mesh.normals.data() });

			mesh.m_vertexbuffer = renderDevice->createVertexBuffer(layout);
			mesh.m_indexbuffer = renderDevice->createIndexBuffer(sizeof(uint32_t), mesh.m_numIndices, import.mesh.indices.data());

			assert(mesh.m_vertexbuffer.isValid());
			assert(mesh.m_indexbuffer.isValid());
		}
		else
		{
			assert(false);
			Logger::errorf("Attempted to load unsuppored Mesh Type %s while loading RenderMesh", path);
		}

		return mesh;
	}

#endif // IMPORT_OBJ_PHI

#if IMPORT_OBJ_TINYOBJ


	// Repurpose ObjIndexer to produce this from tinyobj import
	// Each shape gets written into a submesh. I'm currently making 
	// the assumption that each shape has one material, I should
	// verify that with atleast an assert.
	struct SubMesh
	{
		SubMesh()
			: m_numVertices(0)
			, m_vertices(nullptr)
			, m_normals(nullptr)
			, m_uvs(nullptr)
		{}

		void free()
		{
			delete[] m_vertices;
			delete[] m_normals;
			delete[] m_uvs;
		}

		// Each submesh has m_size verts, norms and uvs (if present)
		// so indexing works properly
		size_t m_numVertices;

		Vec3* m_vertices;
		Vec3* m_normals;
		Vec2* m_uvs;

		int32_t materialID;
	};

	SubMesh convertForOpenGL(float* vertices, float* normals, const tinyobj::shape_t& shape)
	{
		SubMesh mesh;
		size_t numIndices = shape.mesh.indices.size();
		mesh.m_numVertices = numIndices;

		mesh.m_vertices = new Vec3[numIndices];
		mesh.m_normals = new Vec3[numIndices];

		size_t vertsInFace = 3;

		for (size_t indexOffset = 0; indexOffset < numIndices; indexOffset += vertsInFace)
		{
			for (size_t vertex = 0; vertex < vertsInFace; vertex++)
			{
				tinyobj::index_t idx = shape.mesh.indices[indexOffset + vertex];

				Vec3* v = mesh.m_vertices + indexOffset + vertex;
				Vec3* n = mesh.m_normals + indexOffset + vertex;

				v->x = vertices[3 * idx.vertex_index + 0];
				v->y = vertices[3 * idx.vertex_index + 1];
				v->z = vertices[3 * idx.vertex_index + 2];

				n->x = normals[3 * idx.normal_index + 0];
				n->y = normals[3 * idx.normal_index + 1];
				n->z = normals[3 * idx.normal_index + 2];
			}
		}

		return mesh;
	}

	std::vector<SubMesh> loadObj(const char* filename, const char* mtlDir = "Models/sponza/", bool triangulate = true)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, mtlDir, triangulate);
		
		assert(ret);

		Logger::log(err.c_str());

		bool bHasVerts = attrib.vertices.size() > 0;
		bool bHasNorms = attrib.normals.size() > 0;
		bool bHasUvs = attrib.texcoords.size() > 0;

		assert(bHasVerts && bHasNorms);

		std::vector<SubMesh> submeshes;

		for (tinyobj::shape_t shape : shapes)
		{
			SubMesh submesh = convertForOpenGL(attrib.vertices.data(), attrib.normals.data(), shape);
			submeshes.push_back(submesh);
		}

		return submeshes;
	}
	
	std::vector<StaticMesh> loadRenderMesh(const char* path, IRIDevice* renderDevice)
	{
		std::vector<SubMesh> submeshes = loadObj(path);
		
		std::vector<StaticMesh> imports;

		for (SubMesh submesh : submeshes)
		{
			StaticMesh mesh;

			size_t numVertices = submesh.m_numVertices;

			mesh.m_numVertices = numVertices;

			VertexBufferFormat layout;
			layout.add({ EAttributeProperty::Position, EAttributeType::Float, 3 },
			{ sizeof(Vec3), numVertices, submesh.m_vertices });

			layout.add({ EAttributeProperty::Normal, EAttributeType::Float, 3 },
			{ sizeof(Vec3), numVertices, submesh.m_normals });

			mesh.m_vertexbuffer = renderDevice->createVertexBuffer(layout);
			mesh.m_numVertices = submesh.m_numVertices;

			assert(mesh.m_vertexbuffer.isValid());

			imports.push_back(mesh);
		}
		
		return imports;
	}

#endif // IMPORT_OBJ_TINYOBJ
}