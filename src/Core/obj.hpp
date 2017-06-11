#pragma once

#include <vector>
#include <memory>
#include "ObjStructs.hpp"
#include "Math/PhiMath.hpp"

namespace Phoenix
{
	std::unique_ptr<Mesh> loadObj(const std::string& pathTo, const std::string& name);
};

