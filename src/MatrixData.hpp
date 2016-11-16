#pragma once
#include <array>

namespace Phoenix::Math
{
	template<std::size_t N>
	using MatrixData = std::array<std::array<float, N>, N>;
}