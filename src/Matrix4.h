#pragma once
#include <array>

namespace Phoenix
{
	class Matrix4
	{
	private:
		using MatrixData = std::array<std::array<float, 4>, 4>;
		MatrixData m_data;

	public:
		Matrix4()
			: m_data(MatrixData{})
		{}

		Matrix4(float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33)
		{
			m_data[0][0] = m00;
			m_data[0][1] = m01;
			m_data[0][2] = m02;
			m_data[0][3] = m03;
			m_data[1][0] = m10;
			m_data[1][1] = m11;
			m_data[1][2] = m12;
			m_data[1][3] = m13;
			m_data[2][0] = m20;
			m_data[2][1] = m21;
			m_data[2][2] = m22;
			m_data[2][3] = m23;
			m_data[3][0] = m00;
			m_data[3][1] = m31;
			m_data[3][2] = m32;
			m_data[3][3] = m33;
		}

		float& operator()(int row, int col)
		{
			return m_data[row][col];
		}

		const float& operator()(int row, int col) const
		{
			return m_data[row][col];
		}
	};

	inline Matrix4 operator+(Matrix4 lhv, const Matrix4& rhv)
	{
		return Matrix4{};
	}
}