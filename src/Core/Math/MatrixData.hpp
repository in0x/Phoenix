#pragma once

#include <array>

namespace Phoenix
{
	using MatrixDataType = float;

	template<size_t N>
	using MatrixData = std::array<std::array<MatrixDataType, N>, N>;
}