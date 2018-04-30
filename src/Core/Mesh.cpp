#include "Mesh.hpp"

#include <Core/Logger.hpp>
#include <Core/Texture.hpp>
#include <Core/Serialize.hpp>
#include <Core/SerialUtil.hpp>
#include <Core/AssetRegistry.hpp>

#include <Render/RIDevice.hpp>
#include <Render/RIContext.hpp>

#include <assert.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <ThirdParty/tinyobj/tiny_obj_loader.h>

/*
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

		std::string m_name;
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
		mesh.m_meshData.m_numVertices = numIndices;

		mesh.m_meshData.m_vertices.reserve(numIndices);
		mesh.m_meshData.m_normals.reserve(numIndices);
		mesh.m_meshData.m_uvs.reserve(numIndices);

		mesh.m_name = shape.name;

		assert(shape.mesh.num_face_vertices[0] == 3);
		size_t vertsInFace = 3;

		int lastMaterialIdx = -1;

		if (shape.mesh.material_ids[0] == -1)
		{
			Logger::warningf("Shape %s does not have a material, this needs to be assigned a default material!", shape.name.c_str());
			Logger::warning("This shape will be skipped when drawing.");
		}

		assert((numIndices % 3) == 0);

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
				
				matImport.m_name = mtl.name.c_str();
			}

			for (size_t vertex = 0; vertex < vertsInFace; vertex++)
			{
				assert((indexOffset + vertex) < numIndices);

				tinyobj::index_t idx = shape.mesh.indices[indexOffset + vertex];

				size_t vertexIdx = 3 * idx.vertex_index;
				size_t normalIdx = 3 * idx.normal_index;
				size_t uvIdx = 2 * idx.texcoord_index;

				mesh.m_meshData.m_vertices.emplace_back(vertices[vertexIdx + 0], vertices[vertexIdx + 1], vertices[vertexIdx + 2]);
				mesh.m_meshData.m_normals.emplace_back(normals[normalIdx + 0], normals[normalIdx + 1], normals[normalIdx + 2]);
				mesh.m_meshData.m_uvs.emplace_back(uvs[uvIdx + 0], uvs[uvIdx + 1]);
			}
		}

		return mesh;
	}

	// Loads the.obj file and its mtl(s) and converts the mesh into a format drawable by our renderer.
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

		for (const tinyobj::shape_t& shape : shapes)
		{
			MeshImport submesh = convertForRenderApiVNT(attrib.vertices.data(), attrib.normals.data(), attrib.texcoords.data(), shape, materials);
			submeshes.push_back(submesh);
		}

		return submeshes;
	}

	// Creates the mesh's GPU resources.
	void createBuffers(StaticMesh* outMesh, IRIDevice* renderDevice)
	{
		size_t numVertices = outMesh->m_data.m_numVertices;

		VertexBufferFormat layout;
		layout.add({ EAttributeProperty::Position, EAttributeType::Float, 3 },
		{ sizeof(Vec3), numVertices, outMesh->m_data.m_vertices.data() });

		layout.add({ EAttributeProperty::Normal, EAttributeType::Float, 3 },
		{ sizeof(Vec3), numVertices, outMesh->m_data.m_normals.data() });

		layout.add({ EAttributeProperty::TexCoord, EAttributeType::Float, 2 },
		{ sizeof(Vec2), numVertices, outMesh->m_data.m_uvs.data() });

		outMesh->m_vertexbuffer = renderDevice->createVertexBuffer(layout);

		assert(outMesh->m_vertexbuffer.isValid());
	}

	// Fills out our Material structure using the import and creates the necessary GPU resources.
	void createMaterials(const MeshImport& import, StaticMesh* outMesh, const char* mtlPath, AssetRegistry* assets)
	{
		size_t matIdx = 0;
		const char* defaultWhiteTexPath = "Textures/Default1x1White.tga";

		Texture2D* defaultTex = assets->getTexture(defaultWhiteTexPath);

		for (const MaterialImport& matImport : import.m_matImports)
		{
			outMesh->m_vertexFrom[matIdx] = matImport.m_vertexFrom;
			Material* material = &outMesh->m_materials[matIdx];

			material->m_diffuseTex = !matImport.m_diffuseTex.empty() ? assets->getTexture((mtlPath + matImport.m_diffuseTex).c_str()) : defaultTex;
			
			material->m_roughnessTex = !matImport.m_roughnessTex.empty() ? assets->getTexture((mtlPath + matImport.m_roughnessTex).c_str()) : defaultTex;

			material->m_metallicTex = !matImport.m_metallicTex.empty() ? assets->getTexture((mtlPath + matImport.m_metallicTex).c_str()) : defaultTex;
			
			material->m_normalTex = !matImport.m_normalTex.empty() ? assets->getTexture((mtlPath + matImport.m_normalTex).c_str()) : defaultTex;
			
			material->m_name = matImport.m_name;

			matIdx++;
			if (matIdx >= StaticMesh::MAX_MATERIALS)
			{
				Logger::warning("Mesh import exceeds max number of StaticMesh materials. Skipping rest of mtl imports.");
				break;
			}
		}

		// If no mats were imported, assign a default mat.
		if (matIdx == 0)
		{
			outMesh->m_vertexFrom[matIdx] = 0;
			Material* material = &outMesh->m_materials[matIdx];

			material->m_diffuseTex = defaultTex;			
			material->m_roughnessTex = defaultTex;
			material->m_metallicTex = defaultTex;
			material->m_normalTex = defaultTex;
		
			material->m_name = "defaultMaterial";

			matIdx++;
		}

		outMesh->m_numMaterials = static_cast<uint8_t>(std::min(matIdx, (size_t)StaticMesh::MAX_MATERIALS));
	}

	// Loads the .obj file and its mtl(s), converts the mesh into a format drawable by our renderer and creates the GPU resources.
	std::vector<StaticMesh> importObjContents(const char* assetPath, const char* mtlPath, IRIDevice* renderDevice, AssetRegistry* assets)
	{
		std::vector<MeshImport> imports = loadObj(assetPath, mtlPath);

		std::vector<StaticMesh> meshes;

		for (const MeshImport& import : imports)
		{
			meshes.emplace_back();
			StaticMesh* mesh = &meshes.back();
			
			mesh->m_name = import.m_name;
			mesh->m_data = std::move(import.m_meshData);

			createBuffers(mesh, renderDevice);
			createMaterials(import, mesh, mtlPath, assets);
		}

		return meshes;
	}

	std::vector<StaticMesh> importObj(const char* path, IRIDevice* renderDevice, AssetRegistry* assets)
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

			return importObjContents(path, pathToAsset.c_str(), renderDevice, assets);
		}
		else
		{
			assert(false);
			Logger::errorf("Attempted to load unsuppored Mesh Type %s while loading obj", path);
			return{};
		}
	}

	struct MaterialExport
	{
		MaterialExport() = default;

		MaterialExport(const Material& material)
			: m_diffuseTexPath(material.m_diffuseTex->m_sourcePath)
			, m_roughnessTexPath(material.m_roughnessTex->m_sourcePath)
			, m_metallicTexPath(material.m_metallicTex->m_sourcePath)
			, m_normalTexPath(material.m_normalTex->m_sourcePath)
		{}

		std::string m_diffuseTexPath;
		std::string m_roughnessTexPath;
		std::string m_metallicTexPath;
		std::string m_normalTexPath;
	};

	void serialize(Archive& ar, MeshData& data)
	{
		serialize(ar, data.m_numVertices);
		serialize(ar, data.m_vertices);
		serialize(ar, data.m_normals);
		serialize(ar, data.m_uvs);
	}

	void serialize(Archive& ar, Material& material)
	{
		serialize(ar, material.m_name);
	}
	
	void serialize(Archive& ar, StaticMesh& mesh)
	{
		serialize(ar, mesh.m_data);
		serialize(ar, mesh.m_name);
		serialize(ar, mesh.m_numMaterials);

		for (uint8_t i = 0; i < mesh.m_numMaterials; ++i)
		{
			serialize(ar, mesh.m_materials[i]);
			serialize(ar, mesh.m_vertexFrom[i]);
		}
	}

	void serialize(Archive& ar, MaterialExport& exp)
	{
		serialize(ar, exp.m_diffuseTexPath);
		serialize(ar, exp.m_roughnessTexPath);
		serialize(ar, exp.m_metallicTexPath);
		serialize(ar, exp.m_normalTexPath);
	}

	void saveStaticMesh(StaticMesh& mesh, const char* path)
	{
		WriteArchive ar;
		createWriteArchive(sizeof(StaticMesh), &ar);

		serialize(ar, mesh);

		for (uint8_t i = 0; i < mesh.m_numMaterials; ++i)
		{
			MaterialExport exp(mesh.m_materials[i]);
			serialize(ar, exp);
		}
		
		EArchiveError err = writeArchiveToDisk(path, ar);
		assert(err == EArchiveError::NoError);
		destroyArchive(ar);
	}

	StaticMesh loadStaticMesh(const char* path, IRIDevice* renderDevice, AssetRegistry* assets)
	{
		ReadArchive ar;
		StaticMesh mesh;

		EArchiveError err = createReadArchive(path, &ar);

		assert(err == EArchiveError::NoError);
		if (err != EArchiveError::NoError)
		{
			return mesh;
		}

		serialize(ar, mesh);
		createBuffers(&mesh, renderDevice);

		for (uint8_t i = 0; i < mesh.m_numMaterials; ++i)
		{
			MaterialExport exp;
			serialize(ar, exp);

			Material& mat = mesh.m_materials[i];

			mat.m_diffuseTex = assets->getTexture(exp.m_diffuseTexPath.c_str());
			mat.m_roughnessTex = assets->getTexture(exp.m_roughnessTexPath.c_str());
			mat.m_metallicTex = assets->getTexture(exp.m_metallicTexPath.c_str());
			mat.m_normalTex = assets->getTexture(exp.m_normalTexPath.c_str());
		}

		destroyArchive(ar);

		return mesh;
	}
}