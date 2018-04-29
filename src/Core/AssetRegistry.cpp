#include "AssetRegistry.hpp"

#include <Core/Texture.hpp>

namespace Phoenix
{
	AssetRegistry::AssetRegistry(IRIDevice* renderDevice, IRIContext* renderContext)
		: m_renderDevice(renderDevice)
		, m_renderContext(renderContext)
	{}

	AssetRegistry::~AssetRegistry()
	{
		for (Texture2D* tex : m_textures)
		{
			delete tex;
		}
	}
	
	int32_t AssetRegistry::isAssetLoaded(const char* path, EAssetType type)
	{
		auto dictEntry = m_assets.find(path);

		if (dictEntry != m_assets.end() && dictEntry->second.m_type == type)
		{
			return dictEntry->second.m_location;
		}
		else
		{
			return invalidAsset;
		}
	}
	
	Texture2D* AssetRegistry::getTexture(const char* path)
	{
		int32_t location = isAssetLoaded(path, EAssetType::Texture);

		if (location != invalidAsset)
		{
			return m_textures[location];
		}

		Texture2D* tex = new Texture2D();
		Texture2D other = initTextureAsset(path, m_renderDevice, m_renderContext);
		*tex = other;
		
		m_textures.push_back(tex);
		m_assets.emplace(path, AssetRef{ EAssetType::Texture, m_textures.size() - 1 });
		
		return tex;
	}
	
	/*Material* AssetRegistry::getMaterial(const char* path)
	{
	}

	StaticMesh* AssetRegistry::getMesh(const char* path)
	{
	}*/
}