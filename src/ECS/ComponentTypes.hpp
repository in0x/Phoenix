#pragma once

#include <stdint.h>

namespace Phoenix
{
	struct EComponent
	{
		static const uint64_t Transform  = 1 << 0;
		static const uint64_t StaticMesh = 1 << 1;
		static const uint64_t DirLight   = 1 << 2;
		static const uint64_t PointLight = 1 << 3;
	};
}