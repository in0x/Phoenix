#include <cassert>
#include "Matrix4.hpp"
#include "Matrix3.hpp"
#include "Vec4.hpp"
#include "Vec3.hpp"

namespace Phoenix::Math
{	
	Matrix4::Matrix4(float m00, float m01, float m02, float m03,
					 float m10, float m11, float m12, float m13,
					 float m20, float m21, float m22, float m23,
					 float m30, float m31, float m32, float m33)
	{
		m_data[0][0] = m00; m_data[0][1] = m10; m_data[0][2] = m20; m_data[0][3] = m30;
		m_data[1][0] = m01; m_data[1][1] = m11; m_data[1][2] = m21; m_data[1][3] = m31;
		m_data[2][0] = m02; m_data[2][1] = m12; m_data[2][2] = m22; m_data[2][3] = m32;
		m_data[3][0] = m03; m_data[3][1] = m13; m_data[3][2] = m23; m_data[3][3] = m33;
	}

	float& Matrix4::operator()(std::size_t row, std::size_t col)
	{
		if (row >= m_data.size() || row < 0)
		{
			row = 0;
		}
		if (col >= m_data.size() || row < 0)
		{
			col = 0;
		}
		return m_data[col][row];
	}

	const float& Matrix4::operator()(std::size_t row, std::size_t col) const
	{
		if (row >= m_data.size() || row < 0)
		{
			row = 0;
		}
		if (col >= m_data.size() || row < 0)
		{
			col = 0;
		}
		return m_data[col][row];
	}

	Matrix4& Matrix4::operator+=(const Matrix4& rhv)
	{
		Matrix4& lhv = *this;

		for (std::size_t row = 0; row < rows(); row++)
		{
			for (std::size_t col = 0; col < columns(); col++)
			{
				lhv(row, col) += rhv(row, col);
			}
		}

		return *this;
	}

	Matrix4& Matrix4::operator-=(const Matrix4& rhv)
	{
		Matrix4& lhv = *this;

		for (std::size_t row = 0; row < rows(); row++)
		{
			for (std::size_t col = 0; col < columns(); col++)
			{
				lhv(row, col) -= rhv(row, col);
			}
		}

		return *this;
	}

	Matrix4& Matrix4::operator*=(const Matrix4& rhv)
	{
		Matrix4 dest{};
		Matrix4& lhv = *this;
		
		// Transposing the other matrix will improve cache-miss rate.

		for (int i = 0; i <= m_data.size(); i++)
		{
			for (int j = 0; j <= m_data.size(); j++)
			{
				dest(i,j) = lhv(i, 0) * rhv(0, j) + lhv(i, 1) * rhv(1, j) + lhv(i, 2) * rhv(2, j) + lhv(i, 3) * rhv(3, j);
			}
		}

		*this = dest;
		return *this;
	}

	Vec4 Matrix4::operator*(Vec4 rhv) const
	{
		Vec4 dest{};
		const Matrix4& lhv = *this;

		for (int i = 0; i < lhv.rows(); i++)
		{
			dest(i) += lhv(i, 0) * rhv(0) + lhv(i, 1) * rhv(1) + lhv(i, 2) * rhv(2) + lhv(i, 3) * rhv(3);
		}

		return dest;
	}
	
	Vec3 Matrix4::operator*(Vec3 rhv) const
	{
		Vec4 temp{ rhv.x, rhv.y, rhv.z, 0 };
		const Matrix4& lhv = *this;

		temp *= lhv;
		
		rhv.x = temp.x;
		rhv.y = temp.y;
		rhv.z = temp.z;
		return rhv;
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
		Matrix4& lhv = *this;
		auto N = rows();

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				if (lhv(i, j) != rhv(i, j))
				{
					return false;
				}
			}
		}
		return true;
	}

	Matrix4& Matrix4::transposeSelf()
	{
		float temp = 0;
		Matrix4& self = *this;
		for (int i = 0; i < rows(); i++)
		{
			for (int j = i + 1; j < columns(); j++)
			{
				temp = self(i,j);
				self(i,j) = self(j,i);
				self(j,i) = temp;
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
	// Passing the indices of the 3 rows and columns which make up the minor matrix allows us
	// to easily calculate the minor without having to figure out the omitted row and column
	// ourselfs.
	float Matrix4::minor(int row0, int row1, int row2, int col0, int col1, int col2) const
	{
		const Matrix4& self = *this;
		return self(row0,col0) * (self(row1,col1) * self(row2,col2) - self(row1,col2) * self(row2,col1)) -
			   self(row0,col1) * (self(row1,col0) * self(row2,col2) - self(row1,col2) * self(row2,col0)) +
			   self(row0,col2) * (self(row1,col0) * self(row2,col1) - self(row1,col1) * self(row2,col0));
	}

	// Calculate determinant recursively by getting minor for topmost row
	// We choose the topmost row to be omitted, calculate the 3x3 minor submatrix
	// for each element in the topmost row created by omitting the row and column that
	// the current element is in and sum up using the (-1 ^ i + j) pattern to create the
	// cofactors.
	float Matrix4::determinant() const
	{
		const Matrix4& self = *this;
		return self(0,0) * minor(1, 2, 3, 1, 2, 3) -
			   self(0,1) * minor(1, 2, 3, 0, 2, 3) +
			   self(0,2) * minor(1, 2, 3, 0, 1, 3) -
			   self(0,3) * minor(1, 2, 3, 0, 1, 2);
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
		auto det = determinant();
		assert(det);
		return adjoint() / det;
	}

	Matrix4& Matrix4::inverseSelf()
	{
		*this = inverse();
		return *this;
	}

	Matrix3 Matrix4::asMatrix3() const
	{
		const Matrix4& self = *this;

		return Matrix3 {
			self(0,0), self(0,1), self(0,2),
			self(1,0), self(1,1), self(1,2),
			self(2,0), self(2,1), self(2,2)
		};
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

	Matrix4 Matrix4::identity()
	{
		return Matrix4{ 1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1 };
	}

	Vec3 operator*(const Matrix4& lhv, Vec3 rhv)
	{
		return rhv;
	}

	Vec4 operator*(const Matrix4& lhv, Vec4 rhv)
	{
		return rhv;
	}

}