#pragma once

#include <vector>

namespace Phoenix
{
	std::vector<struct StaticMesh*> importObj(const char* path, class IRIDevice* renderDevice, class IRIContext* renderContext, class AssetRegistry* assets);
}