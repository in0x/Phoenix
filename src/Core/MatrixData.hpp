#pragma once
#include <array>
#include "PhiCoreRequired.hpp"

namespace Phoenix
{
	template<int N>
	using MatrixData = std::array<std::array<float, N>, N>;
}