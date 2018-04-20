#include "Mesh.hpp"

#include <Core/Logger.hpp>
#include <Core/Texture.hpp>
#include <Render/RIDevice.hpp>
#include <Render/RIContext.hpp>
#include <Math/Vec2.hpp>

#include <assert.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <ThirdParty/tinyobj/tiny_obj_loader.h>


/*
	TODO:
	Load in UVS

	Possible solution:
	Default uvs if none are present with default textures.
	Not sure how valid this is. I guess I could use a "clear" default texture?
	And then have every uv be 0,0

	Create 1x1 texture with material value if no texture available.

	Or 1x1 white texture? But does that mean that tex and constant values should always be multiplied?
*/

namespace Phoenix
{
	struct MaterialImport
	{
		std::string m_diffuseTex;
		std::string m_roughnessTex;
		std::string m_normalTex;
		std::string m_metallicTex;

		size_t m_vertexFrom;
	};

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
		// so indexing works properly.
		size_t m_numVertices;

		Vec3* m_vertices;
		Vec3* m_normals;
		Vec2* m_uvs;

		std::vector<MaterialImport> m_matImports;
	};

	MeshImport convertForOpenGLVNT(float* vertices, float* normals, float* uvs, const tinyobj::shape_t& shape, const std::vector<tinyobj::material_t>& materials)
	{
		MeshImport mesh;
	
		size_t numIndices = shape.mesh.indices.size();
		mesh.m_numVertices = numIndices;
		mesh.m_vertices = new Vec3[numIndices];
		mesh.m_normals = new Vec3[numIndices];
		mesh.m_uvs = new Vec2[numIndices];

		assert(shape.mesh.num_face_vertices[0] == 3);
		size_t vertsInFace = 3;

		int lastMaterialIdx = -1;

		if (shape.mesh.material_ids[0] == -1)
		{
			Logger::warningf("Shape %s does not have a material, this needs to be assigned a default material!", shape.name.c_str());
			Logger::warning("This shape will be skipped when drawing.");
		}

		for (size_t indexOffset = 0; indexOffset < numIndices; indexOffset += vertsInFace)
		{
			if (shape.mesh.material_ids[indexOffset / 3] != lastMaterialIdx)
			{
				lastMaterialIdx = shape.mesh.material_ids[indexOffset / 3];

				const tinyobj::material_t& mtl = materials[lastMaterialIdx];

				mesh.m_matImports.emplace_back();
				MaterialImport& matImport = mesh.m_matImports.back();

				matImport.m_diffuseTex = mtl.diffuse_texname;
				matImport.m_roughnessTex = mtl.specular_highlight_texname;
				matImport.m_metallicTex = mtl.ambient_texname;
				matImport.m_normalTex = mtl.bump_texname;
				matImport.m_vertexFrom = indexOffset;
			}

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
			MeshImport submesh = convertForOpenGLVNT(attrib.vertices.data(), attrib.normals.data(), attrib.texcoords.data(), shape, materials);
			submeshes.push_back(submesh);
		}

		return submeshes;
	}

	void createBuffers(const MeshImport& import, StaticMesh* outMesh, IRIDevice* renderDevice)
	{
		size_t numVertices = import.m_numVertices;

		outMesh->m_numVertices = numVertices;

		VertexBufferFormat layout;
		layout.add({ EAttributeProperty::Position, EAttributeType::Float, 3 },
		{ sizeof(Vec3), numVertices, import.m_vertices });

		layout.add({ EAttributeProperty::Normal, EAttributeType::Float, 3 },
		{ sizeof(Vec3), numVertices, import.m_normals });

		layout.add({ EAttributeProperty::TexCoord, EAttributeType::Float, 2 },
		{ sizeof(Vec2), numVertices, import.m_uvs });

		outMesh->m_vertexbuffer = renderDevice->createVertexBuffer(layout);
		outMesh->m_numVertices = import.m_numVertices;

		assert(outMesh->m_vertexbuffer.isValid());
	}

	void createMaterials(const MeshImport& import, StaticMesh* outMesh, const char* mtlPath, IRIDevice* renderDevice, IRIContext* renderContext)
	{
		size_t matIdx = 0;

		for (const MaterialImport matImport : import.m_matImports)
		{
			outMesh->m_vertexFrom[matIdx] = matImport.m_vertexFrom;
			Material* material = &outMesh->m_materials[matIdx];

			assert(!matImport.m_diffuseTex.empty());
				
			if (!matImport.m_diffuseTex.empty())
			{
				material->m_diffuseTex = loadRenderTexture2D((mtlPath + matImport.m_diffuseTex).c_str(), "matDiffuseTex", renderDevice, renderContext);
				assert(material->m_diffuseTex.isValid());
			}

			matIdx++;
			if (matIdx >= StaticMesh::MAX_MATERIALS)
			{
				Logger::warning("Mesh import exceeds max number of StaticMesh materials. Skipping rest of mtl imports.");
				break;
			}
		}

		outMesh->m_numMaterials = std::min(matIdx, (size_t)StaticMesh::MAX_MATERIALS);
	}

	std::vector<StaticMesh> importObj(const char* assetPath, const char* mtlPath, IRIDevice* renderDevice, IRIContext* renderContext)
	{
		std::vector<MeshImport> imports = loadObj(assetPath, mtlPath);

		std::vector<StaticMesh> meshes;

		for (MeshImport import : imports)
		{
			meshes.emplace_back();

			createBuffers(import, &meshes.back(), renderDevice);
			createMaterials(import, &meshes.back(), mtlPath, renderDevice, renderContext);

			import.free();
		}

		return meshes;
	}

	std::vector<StaticMesh> loadRenderMesh(const char* path, IRIDevice* renderDevice, IRIContext* renderContext)
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

			return importObj(path, pathToAsset.c_str(), renderDevice, renderContext);
		}
		else
		{
			assert(false);
			Logger::errorf("Attempted to load unsuppored Mesh Type %s while loading Mesh", path);
		}
	}
}