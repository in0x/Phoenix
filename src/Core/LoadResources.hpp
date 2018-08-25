#pragma once

namespace Phoenix
{
	class IRIDevice;
	class IRIContext;
	class AssetRegistry;

	struct LoadResources
	{
		IRIDevice* device;
		IRIContext* context;
		AssetRegistry* assets;
	};
}