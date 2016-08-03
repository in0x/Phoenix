#pragma once
#include <initializer_list>

namespace Phoenix::Math
{
	template<int N>
	class MatrixData
	{
	public:
		MatrixData()
			: m_data(std::array<std::array<float, N>, N>{})
		{
		}

		template<typename ...Elements>
		MatrixData(Elements... elements)
		{
			static_assert(sizeof...(elements) == N, "Not enough elements specified for matrix of this size.");
		}

		decltype(auto) begin()
		{
			return m_data.begin();
		}

		decltype(auto) end()
		{
			return m_data.end();
		}

		decltype(auto) begin() const
		{
			return m_data.begin();
		}

		decltype(auto) end() const
		{
			return m_data.end();
		}

		std::size_t size() const
		{
			return m_data.size();
		}

		float& operator()(std::size_t row, std::size_t col)
		{
			return m_data[row][col];
		}

		const float& operator()(std::size_t row, std::size_t col) const
		{
			return m_data[row][col];
		}

	private:
		std::array<std::array<float, N>, N> m_data;
	};
}