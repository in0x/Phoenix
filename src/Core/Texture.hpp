#pragma once

#include <stdint.h>

#include <Render/RIDefs.hpp>
#include <Render/RIResourceHandles.hpp>

namespace Phoenix
{
	class IRIDevice;
	class IRIContext;

	struct TextureData
	{
		uint8_t* m_data;
		uint32_t m_height;	
		uint32_t m_width;		
		uint8_t m_components;
	};

	struct Texture2D
	{
		std::string m_sourcePath;
		std::string m_resourceName;

		Texture2DHandle m_resourceHandle;
		TextureDesc m_desc;
	};

	void loadTextureData(const char* path, TextureData* outTexture);
	
	// Can optionally be used to unload texture data early.
	// Texture also unloads when destructed.
	void destroyTextureData(TextureData* texture);
	
	void flipTextureHorizontal(TextureData& texture);

	// Loads the texture, creates a GPU resource for it and discards the CPU copy.
	Texture2D createTextureAsset(const char* path, const char* nameInShader, IRIDevice* renderDevice, IRIContext* renderContext);

	// Creates the GPU resources for texture asset. Used for assets that are loaded in from disk.
	void initializeTextureAsset(Texture2D* texture, IRIDevice* renderDevice, IRIContext* renderContext);

	struct Archive;
	void serialize(Archive& ar, Texture2D& texture);
}