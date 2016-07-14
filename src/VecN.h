#pragma once

#include <cmath>
#include <array>
#include <initializer_list>

namespace Phoenix
{
	template<int N>
	class Vec
	{
	public:
		template<class... Args>
		Vec(Args... args)
			: m_data{ args... }
		{}

		int size()
		{
			return N;
		}

		float& operator[] (int i)
		{
			assert(i < N);
			return m_data[i];
		}

		Vec operator+(const Vec& rhv) const
		{
			assert(rhv.size() >= size());
			for (int i = 0; i < N; i++)
				m_data[i] += rhv[i];
		}

	private:
		std::array<float, N> m_data;
	};

	//typedef Vec<3> Vec3f;
}