#pragma once

#include "Texture.hpp"
#include <Core/Logger.hpp>
#include <Core/Serialize.hpp>
#include <Core/SerialUtil.hpp>

#include <Render/RIDevice.hpp>
#include <Render/RIContext.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <ThirdParty/STB/stb_image.h>

namespace Phoenix
{
	struct TextureData
	{
		~TextureData()
		{
			stbi_image_free(m_data);
		}

		uint8_t* m_data;
		uint32_t m_height;
		uint32_t m_width;
		uint8_t m_components;
	};

	static bool loadTextureData(const char* path, TextureData* outTexture)
	{
		//Logger::logf("Loading texture %s", path);

		int width = 0;
		int height = 0;
		int components = 0;

		outTexture->m_data = stbi_load(path, &width, &height, &components, 0);
		
		if (nullptr == outTexture->m_data)
		{
			Logger::errorf("Failed to load texture with error: %s", stbi_failure_reason());
			return false;
		}

		if (((width & (width - 1)) != 0)
			|| ((height & (height - 1)) != 0))
		{
			Logger::warningf("Texture has none power-of-2 dimensions: Width %d, Height: %d", width, height);
		}

		outTexture->m_width = static_cast<uint32_t>(width);
		outTexture->m_height = static_cast<uint32_t>(height);
		outTexture->m_components = static_cast<uint8_t>(components);

		return true;
	}

	static void flipTextureHorizontal(TextureData& texture)
	{
		uint32_t halfHeight = texture.m_height / 2;
		uint32_t widthBytes = texture.m_width * texture.m_components;

		uint8_t* top = nullptr;
		uint8_t* bottom = nullptr;
		uint8_t temp = 0;

		for (size_t row = 0; row < halfHeight; ++row)
		{
			top = texture.m_data + row * widthBytes;
			bottom = texture.m_data + (texture.m_height - row - 1) * widthBytes;

			for (size_t column = 0; column < widthBytes; ++column)
			{
				temp = *bottom;
				*bottom = *top;
				*top = temp;
				top++;
				bottom++;
			}
		}
	}

	TextureDesc createDesc(const TextureData& texture, ETextureFilter minFilter, ETextureFilter magFilter, uint8_t numMips = 0)
	{
		TextureDesc desc;
		desc.width = texture.m_width;
		desc.height = texture.m_height;

		switch (texture.m_components)
		{
		case 4:
		{ desc.pixelFormat = EPixelFormat::R8G8B8A8; } break;
		case 3:
		{ desc.pixelFormat = EPixelFormat::R8G8B8; } break;
		case 2:
		{ desc.pixelFormat = EPixelFormat::RG8; } break;
		case 1:
		{ desc.pixelFormat = EPixelFormat::R8; } break;
		default: { assert(false); } break;
		}

		desc.minFilter = minFilter;
		desc.magFilter = magFilter;

		desc.numMips = numMips;

		desc.wrapU = ETextureWrap::Repeat;
		desc.wrapV = ETextureWrap::Repeat;
		desc.wrapW = ETextureWrap::Repeat;

		return desc;
	}

	bool isLinearSpace(EPixelFormat pf)
	{
		return pf == EPixelFormat::R8G8B8 || pf == EPixelFormat::R8G8B8A8;
	}

	bool isSrgbSpace(EPixelFormat pf)
	{
		return pf == EPixelFormat::SRGB8 || pf == EPixelFormat::SRGBA8;
	}

	EPixelFormat linearToSrgb(EPixelFormat pf)
	{
		switch (pf)
		{
		case EPixelFormat::R8G8B8:
			return EPixelFormat::SRGB8;
		case EPixelFormat::R8G8B8A8:
			return EPixelFormat::SRGBA8;
		default:
			return pf;
		}
	}

	EPixelFormat srgbToLinear(EPixelFormat pf)
	{
		switch (pf)
		{
		case EPixelFormat::SRGB8:
			return EPixelFormat::R8G8B8;
		case EPixelFormat::SRGBA8:
			return EPixelFormat::R8G8B8A8;
		default:
			return pf;
		}
	}

	bool isPow2(uint32_t value)
	{
		return ((value & (value - 1)) == 0);
	}

	uint32_t getMaxMipLevels(uint32_t res)
	{
		return std::log2(res);
	}

	void setDescFromHints(TextureDesc* desc, const TextureCreationHints* hints)
	{
		if (isLinearSpace(desc->pixelFormat) && hints->colorSpace == ETextrueColorSpace::SRGB)
		{
			desc->pixelFormat = linearToSrgb(desc->pixelFormat);
		}
		else if (isSrgbSpace(desc->pixelFormat) && hints->colorSpace == ETextrueColorSpace::Linear)
		{
			desc->pixelFormat = srgbToLinear(desc->pixelFormat);
		}

		desc->magFilter = hints->magFilter;
		desc->minFilter = hints->minFilter;
		desc->wrapU = hints->wrapU;
		desc->wrapV = hints->wrapV;
		desc->wrapW = hints->wrapW;

		if (hints->bGenMipMaps 
			&& desc->width > 1 
			&& desc->height > 1)
		{
			desc->numMips = getMaxMipLevels(std::min(desc->width, desc->height));
			desc->mipFilter = hints->mipFilter;
		}
	}

	Texture2D initTextureAsset(const char* path, const TextureCreationHints* hints, IRIDevice* renderDevice, IRIContext* renderContext)
	{
		Texture2D texture;
		texture.m_sourcePath = path;

		TextureData data;
		if (!loadTextureData(path, &data))
		{
			Logger::warningf("Failed to load data for texture %s. Cannot initialize texture further", path);
			return texture;
		}

		flipTextureHorizontal(data);

		TextureDesc desc = createDesc(data, ETextureFilter::Linear, ETextureFilter::Linear);

		if (hints)
		{
			setDescFromHints(&desc, hints);
		}

		Texture2DHandle tex2D = renderDevice->createTexture2D(desc);
		assert(tex2D.isValid());
		
		renderContext->uploadTextureData(tex2D, data.m_data);
		
		texture.m_resourceHandle = tex2D;
		texture.m_desc = desc;

		return texture;
	}

	void serialize(Archive& ar, TextureDesc& desc)
	{
		ar.serialize(&desc.width, sizeof(uint32_t));
		ar.serialize(&desc.height, sizeof(uint32_t));
		ar.serialize(&desc.pixelFormat, sizeof(EPixelFormat));
		ar.serialize(&desc.minFilter, sizeof(ETextureFilter));
		ar.serialize(&desc.magFilter, sizeof(ETextureFilter));
		ar.serialize(&desc.wrapU, sizeof(ETextureWrap));
		ar.serialize(&desc.wrapV, sizeof(ETextureWrap));
		ar.serialize(&desc.wrapW, sizeof(ETextureWrap));
		ar.serialize(&desc.numMips, sizeof(uint8_t));
	}

	void serialize(Archive& ar, Texture2D& texture)
	{
		serialize(ar, texture.m_desc);
	}
}