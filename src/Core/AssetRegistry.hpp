#pragma once

#include <stdint.h>
#include <vector>
#include <unordered_map>

namespace Phoenix
{
	class IRIDevice;
	class IRIContext;

	enum class EAssetType 
	{
		StaticMesh,
		Material,
		Texture,
		NumAssetTypes
	};

	struct AssetRef
	{
		EAssetType m_type;
		int64_t m_location;
	};

	class AssetRegistry
	{
	public:
		AssetRegistry(IRIDevice* renderDevice, IRIContext* renderContext);
		~AssetRegistry();

		// I can serialize AssetRegistry, thereby easily loading and saving all my assets.
		//static void saveToDisk(const char* pathToArchive);
		//static AssetRegistry loadFromDisk(const char* pathToArchive, IRIDevice* renderDevice, IRIContext* renderContext);

		// TODO: Is this a good interface? Would it be better to have explicit exists, register and get for the user to call?
		struct Texture2D* getTexture(const char* path, const struct TextureCreationHints* hints = nullptr);
	
	private:
		IRIDevice* m_renderDevice;
		IRIContext* m_renderContext;

		int64_t isAssetLoaded(const char* path, EAssetType type);
		const int64_t invalidAsset = -1;

		std::unordered_map<std::string, AssetRef> m_assets;
	
		std::vector<Texture2D*> m_textures;
	};
}