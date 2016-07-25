#pragma once
#include <array>
#include <iostream>

namespace Phoenix::Math
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

		Matrix4(float f)
		{
			for (auto& row : m_data)
			{
				for (auto& el : row)
				{
					el = f;
				}
			}
		}

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

		float& operator()(std::size_t row, std::size_t col)
		{
			return m_data[row][col];
		}

		const float& operator()(std::size_t row, std::size_t col) const
		{
			return m_data[row][col];
		}

		Matrix4& operator+=(const Matrix4& rhv)
		{
			for (std::size_t row = 0; row < m_data.size(); row++)
			{
				for (std::size_t col = 0; col < m_data[row].size(); col++)
				{
					m_data[row][col] += rhv(row, col);
				}
			}

			return *this;
		}

		Matrix4& operator-=(const Matrix4& rhv)
		{
			for (std::size_t row = 0; row < m_data.size(); row++)
			{
				for (std::size_t col = 0; col < m_data[row].size(); col++)
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

		inline Matrix4& operator+=(float f)
		{
			for (auto& row : m_data)
			{
				for (auto& el : row)
				{
					el += f;
				}
			}
			return *this;
		}

		inline Matrix4& operator-=(float f)
		{
			for (auto& row : m_data)
			{
				for (auto& el : row)
				{
					el -= f;
				}
			}
			return *this;
		}

		inline Matrix4& operator*=(float f)
		{
			for (auto& row : m_data)
			{
				for (auto& el : row)
				{
					el *= f;
				}
			}
			return *this;
		}

		inline Matrix4& operator/=(float f)
		{
			for (auto& row : m_data)
			{
				for (auto& el : row)
				{
					el /= f;
				}
			}
			return *this;
		}
		
		bool operator==(const Matrix4& rhv)
		{
			return (m_data[0][0] == rhv(0, 0) && m_data[0][1] == rhv(0, 1) && m_data[0][2] == rhv(0, 2) && m_data[0][3] == rhv(0, 3) &&
					m_data[1][0] == rhv(1, 0) && m_data[1][1] == rhv(1, 1) && m_data[1][2] == rhv(1, 2) && m_data[1][3] == rhv(1, 3) &&
					m_data[2][0] == rhv(2, 0) && m_data[2][1] == rhv(2, 1) && m_data[2][2] == rhv(2, 2) && m_data[2][3] == rhv(2, 3) &&
					m_data[3][0] == rhv(3, 0) && m_data[3][1] == rhv(3, 1) && m_data[3][2] == rhv(3, 2) && m_data[3][3] == rhv(3, 3));
		}

		float determinant() const;

		Matrix4 transpose() const;

		Matrix4 inverse() const;

		static Matrix4 identity();

	private:

		float minor(int row0, int row1, int row2, int col0, int col1, int col2) const;
		
		Matrix4 adjoint() const;		
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

	inline Matrix4 operator+(Matrix4 lhv, float f)
	{
		lhv += f;
		return lhv;
	}

	inline Matrix4 operator-(Matrix4 lhv, float f)
	{
		lhv -= f;
		return lhv;
	}

	inline Matrix4 operator*(Matrix4 lhv, float f)
	{
		lhv *= f;
		return lhv;
	}

	inline Matrix4 operator/(Matrix4 lhv, float f)
	{
		lhv /= f;
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