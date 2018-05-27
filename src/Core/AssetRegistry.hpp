#pragma once

#include <vector>
#include <stdint.h>
#include <unordered_map>

namespace Phoenix
{	
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

		//friend void saveAssetRegistry(AssetRegistry& registry, const char* path);
	};

	//void saveAssetRegistry(AssetRegistry& registry, const char* path);
	//AssetRegistry loadAssetRegistry(const char* path, class IRIDevice* renderDevice, class IRIContext* renderContext);
}