#pragma once

#include "Texture.hpp"
#include <Core/Logger.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "../STB/stb_image.h"

namespace Phoenix
{
	Texture::Texture()
		: m_data(nullptr)
		, m_height(0)
		, m_width(0)
		, m_components(0)
	{}

	Texture::~Texture()
	{
		clear();
	}

	// NOTE(Phil): Consider hiding this in an asset loading service later.
	void Texture::load(const char* path)
	{
		if (nullptr != m_data)
		{
			Logger::log("Texture already loaded, overriding.");
			clear();
		}

		Logger::logf("Loading texture %s", path);

		int width = 0;
		int height = 0;
		int components = 0;

		m_data = stbi_load(path, &width, &height, &components, 0);
		
		if (nullptr == m_data)
		{
			Logger::errorf("Failed to load texture with error: %s", stbi_failure_reason());
			assert(false);
		}

		if (((width & (width - 1)) != 0)
			|| ((height & (height - 1)) != 0))
		{
			Logger::warningf("Texture has none power-of-2 dimensions: Width %d, Height: %d", width, height);
		}

		m_width = static_cast<uint32_t>(width);
		m_height = static_cast<uint32_t>(height);
		m_components = static_cast<uint8_t>(components);
	}

	void Texture::clear()
	{
		if (nullptr != m_data)
		{
			stbi_image_free(m_data);
		}
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
}