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
	void loadTextureData(const char* path, TextureData* outTexture)
	{
		//Logger::logf("Loading texture %s", path);

		int width = 0;
		int height = 0;
		int components = 0;

		outTexture->m_data = stbi_load(path, &width, &height, &components, 0);
		
		if (nullptr == outTexture->m_data)
		{
			Logger::errorf("Failed to load texture with error: %s", stbi_failure_reason());
			assert(false);
		}

		if (((width & (width - 1)) != 0)
			|| ((height & (height - 1)) != 0))
		{
			Logger::warningf("Texture has none power-of-2 dimensions: Width %d, Height: %d", width, height);
		}

		outTexture->m_width = static_cast<uint32_t>(width);
		outTexture->m_height = static_cast<uint32_t>(height);
		outTexture->m_components = static_cast<uint8_t>(components);
	}

	void destroyTextureData(TextureData* texture)
	{
		stbi_image_free(texture->m_data);
		texture->m_data = nullptr;
		texture->m_height = 0;
		texture->m_width = 0;
		texture->m_components = 0;
	}

	void flipTextureHorizontal(TextureData& texture)
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
				*top = *bottom;
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

	Texture2D createTextureAsset(const char* path, const char* nameInShader, IRIDevice* renderDevice, IRIContext* renderContext)
	{
		TextureData data;
		loadTextureData(path, &data);

		TextureDesc desc = createDesc(data, ETextureFilter::Linear, ETextureFilter::Linear);
		Texture2DHandle tex2D = renderDevice->createTexture2D(desc, nameInShader);

		assert(tex2D.isValid());

		renderContext->uploadTextureData(tex2D, data.m_data);

		destroyTextureData(&data);

		Texture2D texture;

		texture.m_resourceName = nameInShader;
		texture.m_resourceHandle = tex2D;
		texture.m_desc = desc;

		texture.m_sourcePath = path;
		
		return texture;
	}

	void initializeTextureAsset(Texture2D* texture, IRIDevice* renderDevice, IRIContext* renderContext)
	{
		TextureData data;
		loadTextureData(texture->m_sourcePath.c_str(), &data);

		TextureDesc desc = createDesc(data, ETextureFilter::Linear, ETextureFilter::Linear);
		Texture2DHandle tex2D = renderDevice->createTexture2D(desc, texture->m_resourceName.c_str());

		assert(tex2D.isValid());

		renderContext->uploadTextureData(tex2D, data.m_data);

		destroyTextureData(&data);

		texture->m_resourceHandle = tex2D;
		texture->m_desc = desc;
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
		serialize(ar, texture.m_sourcePath);
		serialize(ar, texture.m_resourceName);
		serialize(ar, texture.m_desc);
	}
}