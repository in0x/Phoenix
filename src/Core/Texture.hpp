#pragma once

#include <stdint.h>

#include <Render/RIDefs.hpp>
#include <Render/RIResourceHandles.hpp>

namespace Phoenix
{
	class IRIDevice;
	class IRIContext;

	enum class ETextrueColorSpace
	{
		Linear,
		SRGB
	};

	struct TextureCreationHints
	{
		TextureCreationHints()
			: colorSpace(ETextrueColorSpace::Linear)
			, minFilter(ETextureFilter::Linear)
			, magFilter(ETextureFilter::Linear)
			, wrapU(ETextureWrap::ClampToEdge)
			, wrapV(ETextureWrap::ClampToEdge)
			, wrapW(ETextureWrap::ClampToEdge)
		{}

		ETextrueColorSpace colorSpace;
		ETextureFilter magFilter;
		ETextureFilter minFilter;
		ETextureWrap wrapU;
		ETextureWrap wrapV;
		ETextureWrap wrapW;
	};

	struct Texture2D
	{
		std::string m_sourcePath;
		Texture2DHandle m_resourceHandle;
		TextureDesc m_desc;
	};

	// Creates the GPU resources for texture asset. Used for assets that are loaded in from disk.
	// TextureCreationHints may optionally be passed in. Pass nullptr if it should be up to the engine.
	Texture2D initTextureAsset(const char* path, const TextureCreationHints* hints, IRIDevice* renderDevice, IRIContext* renderContext);

	struct Archive;
	void serialize(Archive& ar, Texture2D& texture);
}