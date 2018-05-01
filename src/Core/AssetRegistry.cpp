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
	
	int64_t AssetRegistry::isAssetLoaded(const char* path, EAssetType type)
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
	
	Texture2D* AssetRegistry::getTexture(const char* path, const TextureCreationHints* hints)
	{
		int64_t location = isAssetLoaded(path, EAssetType::Texture);

		if (location != invalidAsset)
		{
			return m_textures[location];
		}

		Texture2D* tex = new Texture2D();
		Texture2D other = initTextureAsset(path, hints, m_renderDevice, m_renderContext);
		*tex = other;
		
		m_textures.push_back(tex);
		m_assets.emplace(path, AssetRef{ EAssetType::Texture, static_cast<int64_t>(m_textures.size() - 1) });
		
		return tex;
	}
}