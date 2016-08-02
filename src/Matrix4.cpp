#include "Matrix4.hpp"

namespace Phoenix::Math
{
	Matrix4& Matrix4::operator+=(const Matrix4& rhv)
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

	Matrix4& Matrix4::operator-=(const Matrix4& rhv)
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

	Matrix4& Matrix4::operator*=(const Matrix4& rhv)
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

	Matrix4& Matrix4::operator+=(float f)
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

	Matrix4& Matrix4::operator-=(float f)
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

	Matrix4& Matrix4::operator*=(float f)
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

	Matrix4& Matrix4::operator/=(float f)
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

	bool Matrix4::operator==(const Matrix4& rhv)
	{
		return (m_data[0][0] == rhv(0, 0) && m_data[0][1] == rhv(0, 1) && m_data[0][2] == rhv(0, 2) && m_data[0][3] == rhv(0, 3) &&
			m_data[1][0] == rhv(1, 0) && m_data[1][1] == rhv(1, 1) && m_data[1][2] == rhv(1, 2) && m_data[1][3] == rhv(1, 3) &&
			m_data[2][0] == rhv(2, 0) && m_data[2][1] == rhv(2, 1) && m_data[2][2] == rhv(2, 2) && m_data[2][3] == rhv(2, 3) &&
			m_data[3][0] == rhv(3, 0) && m_data[3][1] == rhv(3, 1) && m_data[3][2] == rhv(3, 2) && m_data[3][3] == rhv(3, 3));
	}

	Matrix4& Matrix4::transposeSelf()
	{
		float temp;
		for (int i = 0; i < 4; i++)
		{
			for (int j = i + 1; j < 4; j++)
			{
				temp = m_data[i][j];
				m_data[i][j] = m_data[j][i];
				m_data[j][i] = temp;
			}
		}
		return *this;
	}

	Matrix4 Matrix4::transpose() const
	{
		Matrix4 transp = *this;
		transp.transposeSelf();
		return transp;
	}

	// Calculate minor from 3x3 sub matrix 
	float Matrix4::minor(int row0, int row1, int row2, int col0, int col1, int col2) const
	{
		return m_data[row0][col0] * (m_data[row1][col1] * m_data[row2][col2] - m_data[row1][col2] * m_data[row2][col1]) -
			   m_data[row0][col1] * (m_data[row1][col0] * m_data[row2][col2] - m_data[row1][col2] * m_data[row2][col0]) +
			   m_data[row0][col2] * (m_data[row1][col0] * m_data[row2][col1] - m_data[row1][col1] * m_data[row2][col0]);
	}

	// Calculate determinant recursively by getting minor for topmost row
	float Matrix4::determinant() const
	{
		return m_data[0][0] * minor(1, 2, 3, 1, 2, 3) -
			   m_data[0][1] * minor(1, 2, 3, 0, 2, 3) +
			   m_data[0][2] * minor(1, 2, 3, 0, 1, 3) -
			   m_data[0][3] * minor(1, 2, 3, 0, 1, 2);
	}

	// Classical adjoint is transpose of matrix of cofactors
	Matrix4 Matrix4::adjoint() const
	{
		return Matrix4
		{
			  minor(1, 2, 3, 1, 2, 3), -minor(0, 2, 3, 1, 2, 3),  minor(0, 1, 3, 1, 2, 3), -minor(0, 1, 2, 1, 2, 3),
			 -minor(1, 2, 3, 0, 2, 3),  minor(0, 2, 3, 0, 2, 3), -minor(0, 1, 3, 0, 2, 3),  minor(0, 1, 2, 0, 2, 3),	
			  minor(1, 2, 3, 0, 1, 3), -minor(0, 2, 3, 0, 1, 3),  minor(0, 1, 3, 0, 1, 3), -minor(0, 1, 2, 0, 1, 3),
			 -minor(1, 2, 3, 0, 1, 2),  minor(0, 2, 3, 0, 1, 2), -minor(0, 1, 3, 0, 1, 2),  minor(0, 1, 2, 0, 1, 2)
		};
	}

	Matrix4 Matrix4::inverse() const
	{
		// Need to assert that determinant is non-0
		return adjoint() / determinant();
	}

	Matrix4 Matrix4::scale(float x, float y, float z)
	{
		return Matrix4 
		{
			x, 0, 0, 0,
			0, y, 0, 0,
			0, 0, z, 0,
			0, 0, 0, 1
		};
	}

	Matrix4 Matrix4::translation(float x, float y, float z)
	{
		return Matrix4
		{
			0, 0, 0, x,
			0, 0, 0, y,
			0, 0, 0, z,
			0, 0, 0, 1
		};
	}

	Matrix4 Matrix4::rotation(const Quaternion& rotate)
	{
		return Matrix4{};
	}

	Matrix4 Matrix4::identity()
	{
		return Matrix4{ 1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1 };
	}
}