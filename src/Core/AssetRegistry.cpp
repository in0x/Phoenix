#include "AssetRegistry.hpp"

#include <assert.h>

#include <Core/Logger.hpp>
#include <Core/Serialize.hpp>
#include <Core/SerialUtil.hpp>
#include <Core/Texture.hpp>
#include <Core/Material.hpp>
#include <Core/Mesh.hpp>
#include <Core/LoadResources.hpp>

namespace Phoenix
{
	template <typename T>
	int64_t isAssetCached(const char* path, const AssetCache<T>& cache)
	{
		auto dictEntry = cache.m_locations.find(path);

		if (dictEntry != cache.m_locations.end())
		{
			return dictEntry->second;
		}
		else
		{
			return cache.invalidAsset;
		}
	}

	template <typename T>
	T* allocAsset(const char* path, AssetCache<T>& cache)
	{
		T* asset = new T;
		
		cache.m_assets.push_back(asset);
		cache.m_locations.emplace(path, static_cast<int64_t>(cache.m_assets.size() - 1));

		return asset;
	}

	template <typename T>
	T* getAsset(const char* path, AssetCache<T>& cache)
	{
		int64_t location = isAssetCached(path, cache);

		if (location != cache.invalidAsset)
		{
			return cache.m_assets[location];
		}

		return nullptr;
	}

	template <typename T>
	void deleteCache(AssetCache<T>& cache)
	{
		for (T* asset : cache.m_assets)
		{
			delete asset;
		}
	}

	AssetRegistry::~AssetRegistry()
	{
		deleteCache(m_staticMeshCache);
		deleteCache(m_materialCache);
		deleteCache(m_textureCache);
	}

	const char* AssetRegistry::getAssetsPath()
	{
		return "Assets/";
	}

	StaticMesh* AssetRegistry::allocStaticMesh(const char* path)
	{
		return allocAsset(path, m_staticMeshCache);
	}
	
	StaticMesh* AssetRegistry::getStaticMesh(const char* path)
	{
		return getAsset(path, m_staticMeshCache);
	}
	
	Material* AssetRegistry::allocMaterial(const char* path)
	{
		return allocAsset(path, m_materialCache);
	}

	Material* AssetRegistry::getMaterial(const char* path)
	{
		return getAsset(path, m_materialCache);
	}

	Texture2D* AssetRegistry::allocTexture(const char* path)
	{
		return allocAsset(path, m_textureCache);
	}

	Texture2D* AssetRegistry::getTexture(const char* path)
	{
		return getAsset(path, m_textureCache);
	}

	void saveTextureCache(WriteArchive* ar, AssetRegistry* registry, AssetCache<Texture2D>& cache)
	{
		size_t numTex = cache.m_assets.size();
		serialize(ar, numTex);

		for (Texture2D* tex : cache.m_assets)
		{
			serialize(ar, tex->m_name);
			saveTexture(*tex, registry);
		}
	}

	void saveMaterialCache(WriteArchive* ar, AssetRegistry* registry, AssetCache<Material>& cache)
	{
		size_t numMats = cache.m_assets.size();
		serialize(ar, numMats);

		for (Material* mat : cache.m_assets)
		{
			serialize(ar, mat->m_name);
			saveMaterial(*mat, registry);
		}
	}

	void saveMeshCache(WriteArchive* ar, AssetRegistry* registry, AssetCache<StaticMesh>& cache)
	{
		size_t numMeshes = cache.m_assets.size();
		serialize(ar, numMeshes);

		for (StaticMesh* sm : cache.m_assets)
		{
			serialize(ar, sm->m_name);
			saveStaticMesh(*sm, registry);
		}
	}

	void saveAssetRegistry(AssetRegistry& registry, const char* path)
	{
		WriteArchive ar;
		createWriteArchive(sizeof(AssetRegistry), &ar);

		saveTextureCache(&ar, &registry, registry.m_textureCache);
		saveMaterialCache(&ar, &registry, registry.m_materialCache);
		saveMeshCache(&ar, &registry, registry.m_staticMeshCache);

		std::string savePath = registry.getAssetsPath();
		savePath += path;

		EArchiveError err = writeArchiveToDisk(savePath.c_str(), ar);
		assert(err == EArchiveError::NoError);
		destroyArchive(ar);
	}

	void loadTextureCache(ReadArchive* ar, IRIDevice* device, IRIContext* context, AssetRegistry* registry)
	{
		size_t numTex = 0;
		serialize(ar, numTex);

		std::string texName;
		for (size_t i = 0; i < numTex; ++i)
		{
			serialize(ar, texName);
			loadTexture(texName.c_str(), device, context, registry);
		}
	}

	void loadMaterialCache(ReadArchive* ar, IRIDevice* device, IRIContext* context, AssetRegistry* registry)
	{
		size_t numMats = 0;
		serialize(ar, numMats);

		std::string matName;
		for (size_t i = 0; i < numMats; ++i)
		{
			serialize(ar, matName);
			loadMaterial(matName.c_str(), device, context, registry);
		}
	}

	void loadMeshCache(ReadArchive* ar, IRIDevice* device, IRIContext* context, AssetRegistry* registry)
	{
		size_t numMeshes = 0;
		serialize(ar, numMeshes);

		std::string meshName;
		for (size_t i = 0; i < numMeshes; ++i)
		{
			serialize(ar, meshName);
			LoadResources resources{ device, context, registry };
			loadStaticMesh(meshName.c_str(), &resources);
		}
	}

	void loadAssetRegistry(AssetRegistry* outRegistry, const char* path, IRIDevice* renderDevice, IRIContext* renderContext)
	{
		std::string loadPath = outRegistry->getAssetsPath();
		loadPath += path;

		ReadArchive ar;
		EArchiveError err = createReadArchive(loadPath.c_str(), &ar);

		assert(err == EArchiveError::NoError);
		if (err != EArchiveError::NoError)
		{
			Logger::errorf("Failure loading AssetRegistry %s", path);
			return;
		}

		loadTextureCache(&ar, renderDevice, renderContext, outRegistry);
		loadMaterialCache(&ar, renderDevice, renderContext, outRegistry);
		loadMeshCache(&ar, renderDevice, renderContext, outRegistry);

		destroyArchive(ar);
	}
}