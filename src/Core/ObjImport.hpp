#pragma once

#include <vector>

#include <Core/Mesh.hpp>

namespace Phoenix
{
	std::vector<StaticMesh> importObj(const char* path, IRIDevice* renderDevice, IRIContext* renderContext, AssetRegistry* assets);
}