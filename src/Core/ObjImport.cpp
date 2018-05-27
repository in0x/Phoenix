#include "ObjImport.hpp"

#include <Render/RIDevice.hpp>
#include <Render/RIContext.hpp>

#include <Core/Logger.hpp>
#include <Core/Mesh.hpp>
#include <Core/Texture.hpp>
#include <Core/Material.hpp>
#include <Core/AssetRegistry.hpp>

#include <algorithm>

#define TINYOBJLOADER_IMPLEMENTATION
#include <ThirdParty/tinyobj/tiny_obj_loader.h>

namespace Phoenix
{
	struct InitResources
	{
		IRIContext* context;
		IRIDevice* device;
		AssetRegistry* assets;
	};

	struct MaterialImport
	{
		std::string m_name;

		std::string m_diffuseTex;
		std::string m_roughnessTex;
		std::string m_normalTex;
		std::string m_metallicTex;

		size_t m_vertexFrom;
	};

	struct MeshImport
	{
		MeshData m_meshData;
		std::vector<MaterialImport> m_matImports;
		std::string m_name;
	};

	// Converts the mesh into a format usable by rendering APIs i.e. creates linear buffers that have a complete set of values for each vertex.
	MeshImport convertForRenderApiVNT(float* vertices, float* normals, float* uvs, const tinyobj::shape_t& shape, const std::vector<tinyobj::material_t>& materials)
	{
		MeshImport mesh;

		size_t numIndices = shape.mesh.indices.size();

		mesh.m_meshData.setSize(numIndices);
		mesh.m_name = shape.name;

		assert((numIndices % 3) == 0);
		assert(shape.mesh.num_face_vertices[0] == 3);

		size_t vertsInFace = 3;
		size_t lastMaterialIdx = -1;

		MeshData& data = mesh.m_meshData;

		// Convert imported data to engine format.
		for (size_t indexOffset = 0; indexOffset < numIndices; indexOffset += vertsInFace)
		{
			// One shape may have multiple materials. 
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

				matImport.m_name = mtl.name.c_str();
			}

			// Read in vertex, normal and uv pair.
			for (size_t vertex = 0; vertex < vertsInFace; vertex++)
			{
				assert((indexOffset + vertex) < numIndices);

				tinyobj::index_t idx = shape.mesh.indices[indexOffset + vertex];

				size_t vertexIdx = 3 * idx.vertex_index;
				size_t normalIdx = 3 * idx.normal_index;
				size_t uvIdx = 2 * idx.texcoord_index;

				data.m_vertices.emplace_back(vertices[vertexIdx + 0], vertices[vertexIdx + 1], vertices[vertexIdx + 2]);
				data.m_normals.emplace_back(normals[normalIdx + 0], normals[normalIdx + 1], normals[normalIdx + 2]);
				data.m_texCoords.emplace_back(uvs[uvIdx + 0], uvs[uvIdx + 1]);
			}

			// Generate (bi)tangent
			// See: Lengyel, Eric. “Computing Tangent Space Basis Vectors for an Arbitrary Mesh”. Terathon Software, 2001. http://terathon.com/code/tangent.html
			{
				size_t lastVertex = data.m_vertices.size() - 1;

				const Vec3& v3 = data.m_vertices[lastVertex - 2];
				const Vec3& v2 = data.m_vertices[lastVertex - 1];
				const Vec3& v1 = data.m_vertices[lastVertex];

				const Vec2& w3 = data.m_texCoords[lastVertex - 2];
				const Vec2& w2 = data.m_texCoords[lastVertex - 1];
				const Vec2& w1 = data.m_texCoords[lastVertex];

				float x1 = v2.x - v1.x;
				float x2 = v3.x - v1.x;
				float y1 = v2.y - v1.y;
				float y2 = v3.y - v1.y;
				float z1 = v2.z - v1.z;
				float z2 = v3.z - v1.z;

				float s1 = w2.x - w1.x;
				float s2 = w3.x - w1.x;
				float t1 = w2.y - w1.y;
				float t2 = w3.y - w1.y;

				float r = 1.0f / (s1 * t2 - s2 * t1);

				Vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);

				Vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

				for (size_t i = 0; i < vertsInFace; ++i)
				{
					const Vec3& tan1 = sdir;
					const Vec3& tan2 = tdir;
					const Vec3& n = data.m_normals[lastVertex - (vertsInFace - i - 1)];

					Vec3 t;

					// Gram-Schmidt orthogonalize
					t = (tan1 - n * n.dot(tan1));
					t.normalize();

					// Calculate handedness
					float w = n.cross(tan1).dot(tan2) < 0.0f ? -1.0f : 1.0f;

					data.m_tangents.emplace_back(t, w);
				}
			}
		}

		return mesh;
	}

	Texture2D* importMaterialTexture(const std::string& texLocation, const char* basePath, Texture2D* defaultTex, TextureCreationHints* hints, InitResources resources)
	{
		if (!texLocation.empty())
		{
			std::string texPath = (basePath + texPath);
			std::string texName = textureNameFromPath(texPath.c_str());
			Texture2D* tex = resources.assets->getTexture(texName.c_str());

			if (!tex)
			{
				tex = importTexture(texPath.c_str(), hints, resources.device, resources.context, resources.assets);
			}

			return tex;
		}
		else
		{
			return defaultTex;
		}
	}

	Material* importMaterial(const MaterialImport& import, const char* mtlPath, InitResources resources)
	{
		AssetRegistry* assets = resources.assets;

		Texture2D* defaultTexWhite = assets->getTexture(g_defaultWhiteTexPath);
		Texture2D* defaultTexBlack = assets->getTexture(g_defaultBlackTexPath);

		Material* material = assets->allocMaterial(import.m_name.c_str());

		material->m_diffuseTex = importMaterialTexture(import.m_diffuseTex, mtlPath, defaultTexWhite, getDiffuseHints(), resources);
		material->m_roughnessTex = importMaterialTexture(import.m_roughnessTex, mtlPath, defaultTexWhite, getRoughnessHints(), resources);
		material->m_metallicTex = importMaterialTexture(import.m_metallicTex, mtlPath, defaultTexBlack, getMetallicHints(), resources);
		material->m_normalTex = importMaterialTexture(import.m_normalTex, mtlPath, defaultTexBlack, getNormalsHints(), resources);

		material->m_name = import.m_name;

		return material;
	}

	// Fills out our Material structure using the import and creates the necessary GPU resources.
	void createMaterials(const MeshImport& import, StaticMesh* outMesh, const char* mtlPath, InitResources resources)
	{
		size_t matIdx = 0;

		for (const MaterialImport& matImport : import.m_matImports)
		{
			outMesh->m_vertexFrom[matIdx] = matImport.m_vertexFrom;

			Material* material = resources.assets->getMaterial(matImport.m_name.c_str());

			if (material)
			{
				outMesh->m_materials[matIdx] = material;
			}
			else
			{
				outMesh->m_materials[matIdx] = importMaterial(matImport, mtlPath, resources);
			}

			matIdx++;
			if (matIdx >= StaticMesh::MAX_MATERIALS)
			{
				Logger::warning("Mesh import exceeds max number of StaticMesh materials. Skipping rest of mtl imports.");
				break;
			}
		}

		if (matIdx == 0)
		{
			outMesh->m_vertexFrom[matIdx] = 0;
			outMesh->m_materials[matIdx] = resources.assets->getMaterial(g_defaultMaterialPath);
			matIdx++;
		}

		outMesh->m_numMaterials = static_cast<uint8_t>(std::min(matIdx, (size_t)StaticMesh::MAX_MATERIALS));
	}

	// Loads the.obj file and its mtl(s) and converts the mesh into a format drawable by our renderer.
	std::vector<MeshImport> loadObj(const char* filename, const char* mtlDir)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, mtlDir, true);

		assert(ret);

		Logger::warning(err.c_str());

		bool bHasVerts = attrib.vertices.size() > 0;
		bool bHasNorms = attrib.normals.size() > 0;
		bool bHasUvs = attrib.texcoords.size() > 0;

		assert(bHasVerts && bHasNorms && bHasUvs);

		std::vector<MeshImport> submeshes;

		for (const tinyobj::shape_t& shape : shapes)
		{
			MeshImport submesh = convertForRenderApiVNT(attrib.vertices.data(), attrib.normals.data(), attrib.texcoords.data(), shape, materials);
			submeshes.push_back(submesh);
		}

		return submeshes;
	}

	// Loads the .obj file and its mtl(s), converts the mesh into a format drawable by our renderer and creates the GPU resources.
	std::vector<StaticMesh*> importObjContents(const char* assetPath, const char* mtlPath, InitResources resources)
	{
		std::vector<MeshImport> imports = loadObj(assetPath, mtlPath);

		std::vector<StaticMesh*> meshes;

		for (const MeshImport& import : imports)
		{
			StaticMesh* mesh = resources.assets->allocStaticMesh(import.m_name.c_str());
			meshes.push_back(mesh);

			mesh->m_name = import.m_name;
			mesh->m_data = std::move(import.m_meshData);

			createMeshBuffers(mesh, resources.device);
			createMaterials(import, mesh, mtlPath, resources);
		}

		return meshes;
	}

	std::vector<StaticMesh*> importObj(const char* path, IRIDevice* renderDevice, IRIContext* renderContext, AssetRegistry* assets)
	{
		const char* fileDot = strrchr(path, '.');
		size_t pathLen = strlen(path);
		if (nullptr == fileDot)
		{
			assert(false);
			Logger::errorf("Could not locate file ending in path %s while loading Mesh", path);
			return{};
		}

		if (strcmp(".obj", fileDot) == 0)
		{
			const char* lastSlash = strrchr(path, '/');
			lastSlash++;

			std::string pathToAsset(path, lastSlash);
			std::string assetName(lastSlash, path + pathLen);

			InitResources resources{ renderContext, renderDevice, assets };

			return importObjContents(path, pathToAsset.c_str(), resources);
		}
		else
		{
			assert(false);
			Logger::errorf("Attempted to load unsuppored Mesh Type %s while loading obj", path);
			return{};
		}
	}
}