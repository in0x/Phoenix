#pragma once

namespace Phoenix
{


	Texture2D* loadTexture(const char* assetPath, IRIDevice* renderDevice, IRIContext* renderContext, AssetRegistry* assets)
	{
		Texture2D* tex = assets->getTexture(assetPath);

		if (tex)
		{
			return tex;
		}

		ReadArchive ar;
		EArchiveError err = createReadArchive(assetPath, &ar);

		assert(err == EArchiveError::NoError);
		if (err != EArchiveError::NoError)
		{
			return tex;
		}

		TextureAsset asset;
		serialize(ar, asset);

		return importTexture(asset.m_sourcePath.c_str(), &asset.m_hints, renderDevice, renderContext, assets);
	}


	Material* loadMaterial(const char* path, IRIDevice* renderDevice, IRIContext* renderContext, AssetRegistry* assets)
	{
		Material* mat = assets->getMaterial(path);

		if (mat)
		{
			return mat;
		}

		ReadArchive ar;
		EArchiveError err = createReadArchive(path, &ar);

		assert(err == EArchiveError::NoError);
		if (err != EArchiveError::NoError)
		{
			return mat;
		}

		MaterialResources exp;
		serialize(ar, exp);

		mat = assets->allocMaterial(path);
		mat->m_name = exp.m_name;

		mat->m_diffuseTex = loadTexture(exp.m_diffuseTexPath.c_str(), renderDevice, renderContext, assets);
		mat->m_roughnessTex = loadTexture(exp.m_roughnessTexPath.c_str(), renderDevice, renderContext, assets);
		mat->m_metallicTex = loadTexture(exp.m_metallicTexPath.c_str(), renderDevice, renderContext, assets);
		mat->m_normalTex = loadTexture(exp.m_normalTexPath.c_str(), renderDevice, renderContext, assets);

		return mat;
	}


	StaticMesh* loadStaticMesh(const char* path, IRIDevice* renderDevice, IRIContext* renderContext, AssetRegistry* assets)
	{
		StaticMesh* mesh = assets->getStaticMesh(path);

		if (mesh)
		{
			return mesh;
		}

		ReadArchive ar;
		EArchiveError err = createReadArchive(path, &ar);

		assert(err == EArchiveError::NoError);
		if (err != EArchiveError::NoError)
		{
			return mesh;
		}

		mesh = assets->allocStaticMesh(path);

		serialize(ar, *mesh);
		createMeshBuffers(mesh, renderDevice);

		for (uint8_t i = 0; i < mesh->m_numMaterials; ++i)
		{
			MeshMaterialExport exp;
			serialize(ar, exp);

			mesh->m_materials[i] = loadMaterial(exp.m_materialRefs[i].c_str(), renderDevice, renderContext, assets);
		}

		destroyArchive(ar);

		return mesh;
	}
}