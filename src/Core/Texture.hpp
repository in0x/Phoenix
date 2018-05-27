#pragma once

#include <stdint.h>
#include <string>

#include <Render/RIDefs.hpp>
#include <Render/RIResourceHandles.hpp>

namespace Phoenix
{
	static const char* g_defaultWhiteTexName = "Default1x1White";
	static const char* g_defaultBlackTexName = "Default1x1Black";
	static const char* g_defaultWhiteTexPath = "Textures/Default1x1White.tga";
	static const char* g_defaultBlackTexPath = "Textures/Default1x1Black.tga";

	class IRIDevice;
	class IRIContext;
	class AssetRegistry;

	enum class ETextrueColorSpace
	{
		Linear,
		SRGB
	};

	struct TextureCreationHints
	{
		TextureCreationHints()
			: colorSpace(ETextrueColorSpace::Linear)
			, minFilter(ETextureFilter::Nearest)
			, magFilter(ETextureFilter::Nearest)
			, mipFilter(ETextureFilter::Nearest)
			, wrapU(ETextureWrap::ClampToEdge)
			, wrapV(ETextureWrap::ClampToEdge)
			, wrapW(ETextureWrap::ClampToEdge)
			, bGenMipMaps(false)
		{}

		// What color space the texture should be treated to be in. Linear or SRGB.
		ETextrueColorSpace colorSpace;

		// What filter mode should be used when magnifying.
		ETextureFilter magFilter;

		// What filter mode should be used when minifying.
		ETextureFilter minFilter;
		
		// What filter should be used for mip levels. Only applies when bAutoMipMap
		// is true. mipFilter is used when sampling between mip levels, while minFilter 
		// is still used inside of the chosen mip level.
		ETextureFilter mipFilter;

		// How texcoords outside of [0..1] should be interpreted in U direction. 
		ETextureWrap wrapU;
		
		// How texcoords outside of [0..1] should be interpreted in V direction. 
		ETextureWrap wrapV;
		
		// How texcoords outside of [0..1] should be interpreted in W direction.
		// Only applies to 3D textures.
		ETextureWrap wrapW;
		
		// Whether to generate a mip-map for this texture.
		bool bGenMipMaps;
	};

	struct Texture2D
	{
		std::string m_name;
		std::string m_sourcePath;
		TextureDesc m_desc;
		Texture2DHandle m_resourceHandle;
		TextureCreationHints m_creationHints;
	};

	std::string textureNameFromPath(const char* path);

	// Creates the GPU resources for texture asset. Used for assets that are loaded in from disk.
	// TextureCreationHints may optionally be passed in. Pass nullptr if it should be up to the engine.
	Texture2D* importTexture(const char* imagePath, const TextureCreationHints* hints, IRIDevice* renderDevice, IRIContext* renderContext, AssetRegistry* assets);
	
	Texture2D* loadTexture(const char* path, IRIDevice* renderDevice, IRIContext* renderContext, AssetRegistry* assets);

	void saveTexture(const Texture2D& texture, AssetRegistry* assets);
}