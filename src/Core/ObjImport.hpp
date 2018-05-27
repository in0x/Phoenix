#pragma once

#include <vector>

namespace Phoenix
{
	struct StaticMesh;
	class IRIDevice;
	class IRIContext;
	class AssetRegistry;

	std::vector<StaticMesh*> importObj(const char* path, IRIDevice* renderDevice, IRIContext* renderContext, AssetRegistry* assets);
}