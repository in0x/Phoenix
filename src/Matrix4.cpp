#include "Matrix4.h"

namespace Phoenix
{
	Matrix4 Matrix4::transpose() const
	{
		return Matrix4{ m_data[0][0], m_data[1][0], m_data[2][0], m_data[3][0],
						m_data[0][1], m_data[1][1], m_data[2][1], m_data[3][1],
						m_data[0][2], m_data[1][2], m_data[2][2], m_data[3][2],
						m_data[0][3], m_data[1][3], m_data[2][3], m_data[3][3] };
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

	Matrix4 Matrix4::identity()
	{
		return Matrix4{ 1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1 };
	}
}