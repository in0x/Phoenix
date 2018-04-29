#pragma once

#include <stdint.h>

#include <Render/RIDefs.hpp>
#include <Render/RIResourceHandles.hpp>

namespace Phoenix
{
	class IRIDevice;
	class IRIContext;

	struct Texture2D
	{
		std::string m_sourcePath;
		Texture2DHandle m_resourceHandle;
		TextureDesc m_desc;
	};

	// Creates the GPU resources for texture asset. Used for assets that are loaded in from disk.
	Texture2D initTextureAsset(const char* path, IRIDevice* renderDevice, IRIContext* renderContext);

	struct Archive;
	void serialize(Archive& ar, Texture2D& texture);
}