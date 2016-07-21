#pragma once
#include <array>
#include <iostream>

namespace Phoenix
{
	class Matrix4
	{
	private:
		// STL Container, should be contigous, but i should do a 
		// bit more digging on std::array memory layout
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
			m_data[0][0] = m00; m_data[0][1] = m01; m_data[0][2] = m02; m_data[0][3] = m03;
			m_data[1][0] = m10; m_data[1][1] = m11; m_data[1][2] = m12; m_data[1][3] = m13;
			m_data[2][0] = m20; m_data[2][1] = m21; m_data[2][2] = m22; m_data[2][3] = m23;
			m_data[3][0] = m30; m_data[3][1] = m31; m_data[3][2] = m32; m_data[3][3] = m33;
		}

		float& operator()(int row, int col)
		{
			return m_data[row][col];
		}

		const float& operator()(int row, int col) const
		{
			return m_data[row][col];
		}

		Matrix4& operator+=(const Matrix4& rhv)
		{
			for (int row = 0; row < m_data.size(); row++)
			{
				for (int col = 0; col < m_data[row].size(); col++)
				{
					m_data[row][col] += rhv(row, col);
				}
			}

			return *this;
		}

		Matrix4& operator-=(const Matrix4& rhv)
		{
			for (int row = 0; row < m_data.size(); row++)
			{
				for (int col = 0; col < m_data[row].size(); col++)
				{
					m_data[row][col] -= rhv(row, col);
				}
			}

			return *this;
		}

		Matrix4& operator*=(const Matrix4& rhv)
		{
			m_data[0][0] = m_data[0][0] * rhv(0, 0) + m_data[0][1] * rhv(1, 0) + m_data[0][2] * rhv(2, 0) + m_data[0][3] * rhv(3, 0);
			m_data[0][1] = m_data[0][0] * rhv(0, 1) + m_data[0][1] * rhv(1, 1) + m_data[0][2] * rhv(2, 1) + m_data[0][3] * rhv(3, 1);
			m_data[0][2] = m_data[0][0] * rhv(0, 2) + m_data[0][1] * rhv(1, 2) + m_data[0][2] * rhv(2, 2) + m_data[0][3] * rhv(3, 2);
			m_data[0][3] = m_data[0][0] * rhv(0, 3) + m_data[0][1] * rhv(1, 3) + m_data[0][2] * rhv(2, 3) + m_data[0][3] * rhv(3, 3);

			m_data[1][0] = m_data[1][0] * rhv(0, 0) + m_data[1][1] * rhv(1, 0) + m_data[1][2] * rhv(2, 0) + m_data[1][3] * rhv(3, 0);
			m_data[1][1] = m_data[1][0] * rhv(0, 1) + m_data[1][1] * rhv(1, 1) + m_data[1][2] * rhv(2, 1) + m_data[1][3] * rhv(3, 1);
			m_data[1][2] = m_data[1][0] * rhv(0, 2) + m_data[1][1] * rhv(1, 2) + m_data[1][2] * rhv(2, 2) + m_data[1][3] * rhv(3, 2);
			m_data[1][3] = m_data[1][0] * rhv(0, 3) + m_data[1][1] * rhv(1, 3) + m_data[1][2] * rhv(2, 3) + m_data[1][3] * rhv(3, 3);

			m_data[2][0] = m_data[2][0] * rhv(0, 0) + m_data[2][1] * rhv(1, 0) + m_data[2][2] * rhv(2, 0) + m_data[2][3] * rhv(3, 0);
			m_data[2][1] = m_data[2][0] * rhv(0, 1) + m_data[2][1] * rhv(1, 1) + m_data[2][2] * rhv(2, 1) + m_data[2][3] * rhv(3, 1);
			m_data[2][2] = m_data[2][0] * rhv(0, 2) + m_data[2][1] * rhv(1, 2) + m_data[2][2] * rhv(2, 2) + m_data[2][3] * rhv(3, 2);
			m_data[2][3] = m_data[2][0] * rhv(0, 3) + m_data[2][1] * rhv(1, 3) + m_data[2][2] * rhv(2, 3) + m_data[2][3] * rhv(3, 3);

			m_data[3][0] = m_data[3][0] * rhv(0, 0) + m_data[3][1] * rhv(1, 0) + m_data[3][2] * rhv(2, 0) + m_data[3][3] * rhv(3, 0);
			m_data[3][1] = m_data[3][0] * rhv(0, 1) + m_data[3][1] * rhv(1, 1) + m_data[3][2] * rhv(2, 1) + m_data[3][3] * rhv(3, 1);
			m_data[3][2] = m_data[3][0] * rhv(0, 2) + m_data[3][1] * rhv(1, 2) + m_data[3][2] * rhv(2, 2) + m_data[3][3] * rhv(3, 2);
			m_data[3][3] = m_data[3][0] * rhv(0, 3) + m_data[3][1] * rhv(1, 3) + m_data[3][2] * rhv(2, 3) + m_data[3][3] * rhv(3, 3);

			return *this;
		}

		void transpose()
		{
		
		}

		void invert()
		{
		
		}

		static Matrix4 unit()
		{
			return Matrix4 { 1, 0, 0, 0,
							 0, 1, 0, 0,
							 0, 0, 1, 0,
							 0, 0, 0, 1 };
		}
	};

	inline Matrix4 operator+(Matrix4 lhv, const Matrix4& rhv)
	{
		lhv += rhv;
		return lhv;
	}

	inline Matrix4 operator-(Matrix4 lhv, const Matrix4& rhv)
	{
		lhv -= rhv;
		return lhv;
	}

	inline Matrix4 operator*(Matrix4 lhv, const Matrix4& rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	inline std::ostream& operator<<(std::ostream& out, const Matrix4& mat4)
	{
		std::cout << mat4(0, 0) << " " << mat4(0, 1) << " " << mat4(0, 2) << " " << mat4(0, 3) << "\n"
			  	  << mat4(1, 0) << " " << mat4(1, 1) << " " << mat4(1, 2) << " " << mat4(1, 3) << "\n"
				  << mat4(2, 0) << " " << mat4(2, 1) << " " << mat4(2, 2) << " " << mat4(2, 3) << "\n"
				  << mat4(3, 0) << " " << mat4(3, 1) << " " << mat4(3, 2) << " " << mat4(3, 3);
		return out;
	}
}