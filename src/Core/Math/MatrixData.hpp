#pragma once

#include <array>

namespace Phoenix
{
	template<int N>
	using MatrixData = std::array<std::array<float, N>, N>;
}