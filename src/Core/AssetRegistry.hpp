#pragma once

#include <vector>
#include <stdint.h>
#include <unordered_map>

// Circular include here because AssetCache needs the definitions to be able to new / delete
// But all the asset cpps include AssetRegistry -> end up including themselves

namespace Phoenix
{	
	//template <typename Asset>
	//struct AssetCache
	//{
	//	~AssetCache()
	//	{
	//		for (Asset* asset : m_assets)
	//		{
	//			delete asset;
	//		}
	//	}

	//	const int64_t invalidAsset = -1;

	//	int64_t isAssetCached(const char* path)
	//	{
	//		auto dictEntry = m_locations.find(path);

	//		if (dictEntry != m_locations.end())
	//		{
	//			return dictEntry->second;
	//		}
	//		else
	//		{
	//			return invalidAsset;
	//		}
	//	}

	//	Asset* alloc(const char* path)
	//	{
	//		Asset* asset = new Asset;

	//		m_assets.push_back(asset);
	//		m_locations.emplace(path, static_cast<int64_t>(m_assets.size() - 1));

	//		return asset;
	//	}

	//	Asset* get(const char* path)
	//	{
	//		int64_t location = isAssetCached(path);

	//		if (location != invalidAsset)
	//		{
	//			return m_assets[location];
	//		}

	//		return nullptr;
	//	}

	//	std::unordered_map<std::string, int64_t> m_locations;
	//	std::vector<Asset*> m_assets;
	//};

	/*struct AssetRegistry
	{
		AssetCache<Texture2D> m_textureCache;
		AssetCache<StaticMesh> m_staticMeshCache;
		AssetCache<Material> m_materialCache;
	};*/

	struct StaticMesh;
	struct Material;
	struct Texture2D;
	
	template<typename Asset>
	struct AssetCache
	{
		const int64_t invalidAsset = -1;
		std::unordered_map<std::string, int64_t> m_locations;
		std::vector<Asset*> m_assets;
	};

	class AssetRegistry
	{
	public:
		~AssetRegistry();

		StaticMesh* allocStaticMesh(const char* path);
		StaticMesh* getStaticMesh(const char* path);

		Material* allocMaterial(const char* path);
		Material* getMaterial(const char* path);

		Texture2D* allocTexture(const char* path);
		Texture2D* getTexture(const char* path);

	private:
		AssetCache<StaticMesh> m_staticMeshCache;
		AssetCache<Material> m_materialCache;
		AssetCache<Texture2D> m_textureCache;

		friend void saveAssetRegistry(AssetRegistry& registry, const char* path);
	};

	void saveAssetRegistry(AssetRegistry& registry, const char* path);
	AssetRegistry loadAssetRegistry(const char* path, class IRIDevice* renderDevice, class IRIContext* renderContext);
}