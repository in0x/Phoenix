#pragma once

#include <stdint.h>

namespace Phoenix
{
	enum class AssetType : int32_t
	{
		StaticMesh,
		Material,
		Texture,
		NumAssetTypes
	};

	/*
		Anything ending with .phi is considered an asset
		.phi files are just binary archives of the asset
		
		The first value is always the AssetType
		Load as a int32_t so we can safely identify invalid values 
		Unless of course they end up with a valid, in which case were just out of luck
		This problem can probably atleast be partially solved with an AssetRegistry   
		that hands out IDs which could be used to check if the asset is good.

		Assets will need to reference other assets somehow. The easiest way is probably
		using names/paths. 

		Load assets with path
		Simpler fixed world with enum component types so we can binary serialize it
		allows me to serialize entities, which solve problem with multiple meshes from one obj
	*/

	/*struct TextureAsset
	{
		Texture2DHandle handle;

	};*/
}