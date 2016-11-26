#pragma once
#include <array>
#include "PhiCoreRequired.hpp"

namespace Phoenix
{
	template<int32 N>
	using MatrixData = std::array<std::array<f32, N>, N>;
}