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

	float Matrix4::determinant() const
	{
	
	}

	Matrix4 Matrix4::adjoint() const
	{
	
	}

	Matrix4 Matrix4::inverse() const
	{
	
	}

	static Matrix4 identity()
	{
		return Matrix4{ 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 };
	}
}