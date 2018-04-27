#pragma once

#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Phoenix
{
	class IRIDevice;
	class IRIContext;

	enum class AssetType 
	{
		StaticMesh,
		Material,
		Texture,
		NumAssetTypes
	};


	class AssetRegistry
	{
		AssetRegistry(IRIDevice* renderDevice, IRIContext* renderContext);

		// I can serialize AssetRegistry, thereby easily loading and saving all my assets.
		//static void saveToDisk(const char* pathToArchive);
		//static AssetRegistry loadFromDisk(const char* pathToArchive, IRIDevice* renderDevice, IRIContext* renderContext);

		const class Texture2D* getTexture(const char* path);
	/*	const class Material* getMaterial(const char* path);
		const class StaticMesh* getMesh(const char* path);
	*/
	private:
		IRIDevice* m_renderDevice;
		IRIContext* m_renderContext;

		struct AssetEntry
		{
			AssetType m_type;
			size_t m_location;
		};

		int32_t isAssetLoaded(const char* path, AssetType type);
		const int32_t invalidAsset = -1;

		std::unordered_map<std::string, AssetEntry> m_assets;
	
		std::vector<std::unique_ptr<Texture2D>> m_textures;
		/*std::vector<std::unique_ptr<Material>> m_materials;
		std::vector<std::unique_ptr<StaticMesh>> m_meshes;*/
	};
}