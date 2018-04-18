#include "Mesh.hpp"

#include <Core/Logger.hpp>
#include <Render/RIDevice.hpp>

#include <assert.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <ThirdParty/tinyobj/tiny_obj_loader.h>

#include <Math/Vec3.hpp>
#include <Math/Vec2.hpp>

/*
	TODO:
	Load in UVS

	Possible solution:
	Default uvs if none are present with default textures.
	Not sure how valid this is. I guess I could use a "clear" default texture?
	And then have every uv be 0,0

	Then the default pbr values get blended with the texture values?

	If no texture present, use a default 1x1 pixel white texture. Then multiply with material value.
*/

namespace Phoenix
{
	// I'm currently making 
	// the assumption that each shape has one material, I should
	// verify that with atleast an assert.
	struct MeshImport
	{
		MeshImport()
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

	MeshImport convertForOpenGL(float* vertices, float* normals, float* uvs, const tinyobj::shape_t& shape)
	{
		MeshImport mesh;
	
		size_t numIndices = shape.mesh.indices.size();
		mesh.m_numVertices = numIndices;
		mesh.m_vertices = new Vec3[numIndices];
		mesh.m_normals = new Vec3[numIndices];
		mesh.m_uvs = new Vec2[numIndices];

		assert(shape.mesh.num_face_vertices[0] == 3);
		size_t vertsInFace = 3;

		for (size_t indexOffset = 0; indexOffset < numIndices; indexOffset += vertsInFace)
		{
			for (size_t vertex = 0; vertex < vertsInFace; vertex++)
			{
				tinyobj::index_t idx = shape.mesh.indices[indexOffset + vertex];

				Vec3* v = mesh.m_vertices + indexOffset + vertex;
				Vec3* n = mesh.m_normals + indexOffset + vertex;
				Vec2* t = mesh.m_uvs + indexOffset + vertex;

				size_t vertexIdx = 3 * idx.vertex_index;
				v->x = vertices[vertexIdx + 0];
				v->y = vertices[vertexIdx + 1];
				v->z = vertices[vertexIdx + 2];

				size_t normalIdx = 3 * idx.normal_index;
				n->x = normals[normalIdx + 0];
				n->y = normals[normalIdx + 1];
				n->z = normals[normalIdx + 2];

				size_t uvIdx = 2 * idx.texcoord_index;
				t->x = uvs[uvIdx + 0];
				t->y = uvs[uvIdx + 1];
			}
		}

		return mesh;
	}

	std::vector<MeshImport> loadObj(const char* filename, const char* mtlDir = nullptr, bool triangulate = true)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, mtlDir, triangulate);

		assert(ret);

		Logger::warning(err.c_str());

		bool bHasVerts = attrib.vertices.size() > 0;
		bool bHasNorms = attrib.normals.size() > 0;
		bool bHasUvs = attrib.texcoords.size() > 0;

		assert(bHasVerts && bHasNorms && bHasUvs);

		std::vector<MeshImport> submeshes;

		for (tinyobj::shape_t shape : shapes)
		{
			MeshImport submesh = convertForOpenGL(attrib.vertices.data(), attrib.normals.data(), attrib.texcoords.data(), shape);
			submeshes.push_back(submesh);
		}

		// TODO: Patch uvs if not existant

		return submeshes;
	}

	std::vector<StaticMesh> importObj(const char* assetPath, const char* mtlPath, IRIDevice* renderDevice)
	{
		std::vector<MeshImport> imports = loadObj(assetPath, mtlPath);

		std::vector<StaticMesh> meshes;

		for (MeshImport import : imports)
		{
			StaticMesh mesh;

			size_t numVertices = import.m_numVertices;

			mesh.m_numVertices = numVertices;

			VertexBufferFormat layout;
			layout.add({ EAttributeProperty::Position, EAttributeType::Float, 3 },
			{ sizeof(Vec3), numVertices, import.m_vertices });

			layout.add({ EAttributeProperty::Normal, EAttributeType::Float, 3 },
			{ sizeof(Vec3), numVertices, import.m_normals });

			layout.add({ EAttributeProperty::TexCoord, EAttributeType::Float, 2 },
			{ sizeof(Vec2), numVertices, import.m_uvs });

			mesh.m_vertexbuffer = renderDevice->createVertexBuffer(layout);
			mesh.m_numVertices = import.m_numVertices;

			assert(mesh.m_vertexbuffer.isValid());

			meshes.push_back(mesh);

			import.free();
		}

		return meshes;
	}

	std::vector<StaticMesh> loadRenderMesh(const char* path, IRIDevice* renderDevice)
	{
		const char* fileDot = strrchr(path, '.');
		size_t pathLen = strlen(path);
		if (nullptr == fileDot)
		{
			assert(false);
			Logger::errorf("Could not locate file ending in path %s while loading Mesh", path);
			return {};
		}

		if (strcmp(".obj", fileDot) == 0)
		{
			const char* lastSlash = strrchr(path, '/');
			lastSlash++;

			std::string pathToAsset(path, lastSlash);
			std::string assetName(lastSlash, path + pathLen);

			return importObj(path, pathToAsset.c_str(), renderDevice);
		}
		else
		{
			assert(false);
			Logger::errorf("Attempted to load unsuppored Mesh Type %s while loading Mesh", path);
		}
	}
}