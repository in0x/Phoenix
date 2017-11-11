#pragma once

#include <stdint.h>

#include <Render/RIResourceHandles.hpp>

namespace Phoenix
{
	class IRIDevice;

	class Texture
	{
	public:
		Texture();
		~Texture();

		uint8_t* m_data;
		uint32_t m_height;	
		uint32_t m_width;		
		uint8_t m_components;
	};

	Texture loadTexture(const char* path);
	
	// Can optionally be used to unload texture data early.
	// Texture also unloads when destructed.
	void destroyTexture(Texture& texture);
	
	void flipTextureHorizontal(Texture& texture);

	Texture2DHandle loadRenderTexture2D(const char* path, const char* nameInShader, IRIDevice* renderDevice);
}