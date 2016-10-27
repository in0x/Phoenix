#pragma once
#include <array>

namespace phoenix::math
{
	template<size_t N>
	using matrixData = std::array<std::array<float, N>, N>;
}