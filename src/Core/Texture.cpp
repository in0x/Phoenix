#pragma once

#include "Texture.hpp"
#include <Core/Logger.hpp>

#include <Render/RIDefs.hpp>
#include <Render/RIDevice.hpp>
#include <Render/RIContext.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <ThirdParty/STB/stb_image.h>

namespace Phoenix
{
	void loadTexture(const char* path, Texture* outTexture)
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

	void destroyTexture(Texture* texture)
	{
		stbi_image_free(texture->m_data);
		texture->m_data = nullptr;
		texture->m_height = 0;
		texture->m_width = 0;
		texture->m_components = 0;
	}

	void flipTextureHorizontal(Texture& texture)
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

	TextureDesc createDesc(const Texture& texture, ETextureFilter minFilter, ETextureFilter magFilter, uint8_t numMips = 0)
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
		{ assert(false); } break;
		case 1:
		{ assert(false); } break;
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

	Texture2DHandle loadRenderTexture2D(const char* path, const char* nameInShader, IRIDevice* renderDevice, IRIContext* renderContext)
	{
		Texture tex;
		loadTexture(path, &tex);

		TextureDesc desc = createDesc(tex, ETextureFilter::Linear, ETextureFilter::Linear);
		Texture2DHandle tex2D = renderDevice->createTexture2D(desc, nameInShader);

		assert(tex2D.isValid());

		renderContext->uploadTextureData(tex2D, tex.m_data);

		destroyTexture(&tex);

		return tex2D;
	}
}