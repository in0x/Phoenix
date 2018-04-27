#include "AssetRegistry.hpp"

#include <Core/Texture.hpp>

namespace Phoenix
{
	AssetRegistry::AssetRegistry(IRIDevice* renderDevice, IRIContext* renderContext)
		: m_renderDevice(renderDevice)
		, m_renderContext(renderContext)
	{}
	
	int32_t AssetRegistry::isAssetLoaded(const char* path, AssetType type)
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
	
	const Texture2D* AssetRegistry::getTexture(const char* path)
	{
		//int32_t location = isAssetLoaded(path, AssetType::Texture);
		//
		//if (location != invalidAsset)
		//{
		//	return m_textures[location].get();
		//}

		//createTextureAsset(path)
		
		return nullptr;
	}
	
	/*Material* AssetRegistry::getMaterial(const char* path)
	{
	}

	StaticMesh* AssetRegistry::getMesh(const char* path)
	{
	}*/
}