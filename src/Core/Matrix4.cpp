#include "Matrix4.hpp"
#include <cassert>
#include "Common.hpp"
#include "Matrix3.hpp"
#include "Vec4.hpp"
#include "Vec3.hpp"
#include "EulerAngles.hpp"

namespace Phoenix
{
	Matrix4::Matrix4(f32 m00, f32 m01, f32 m02, f32 m03,
		f32 m10, f32 m11, f32 m12, f32 m13,
		f32 m20, f32 m21, f32 m22, f32 m23,
		f32 m30, f32 m31, f32 m32, f32 m33)
	{
		m_data[0][0] = m00; m_data[0][1] = m10; m_data[0][2] = m20; m_data[0][3] = m30;
		m_data[1][0] = m01; m_data[1][1] = m11; m_data[1][2] = m21; m_data[1][3] = m31;
		m_data[2][0] = m02; m_data[2][1] = m12; m_data[2][2] = m22; m_data[2][3] = m32;
		m_data[3][0] = m03; m_data[3][1] = m13; m_data[3][2] = m23; m_data[3][3] = m33;
	}

	f32& Matrix4::operator()(int32 row, int32 col)
	{
		return m_data[col][row];
	}

	const f32 Matrix4::operator()(int32 row, int32 col) const
	{
		return m_data[col][row];
	}

	Matrix4& Matrix4::operator+=(const Matrix4& rhv)
	{
		Matrix4& lhv = *this;

		for (int32 row = 0; row < 4; row++)
		{
			for (int32 col = 0; col < 4; col++)
			{
				lhv(row, col) += rhv(row, col);
			}
		}

		return *this;
	}

	Matrix4& Matrix4::operator-=(const Matrix4& rhv)
	{
		Matrix4& lhv = *this;

		for (int32 row = 0; row < 4; row++)
		{
			for (int32 col = 0; col < 4; col++)
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

		for (int32 row = 0; row < 4; row++)
		{
			for (int32 col = 0; col < 4; col++)
			{
				dest(row, col) = lhv(row, 0) * rhv(0, col) + lhv(row, 1) * rhv(1, col) + lhv(row, 2) * rhv(2, col) + lhv(row, 3) * rhv(3, col);
			}
		}

		*this = dest;
		return *this;
	}

	Vec4 Matrix4::operator*(Vec4 rhv) const
	{
		Vec4 dest{};
		const Matrix4& lhv = *this;

		for (int32 i = 0; i < 4; i++)
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

	Matrix4& Matrix4::operator+=(f32 f)
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

	Matrix4& Matrix4::operator-=(f32 f)
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

	Matrix4& Matrix4::operator*=(f32 f)
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

	Matrix4& Matrix4::operator/=(f32 f)
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
		
		for (int32 i = 0; i < 4; i++)
		{
			for (int32 j = 0; j < 4; j++)
			{
				if (!almostEqualRelative(lhv(i, j), rhv(i, j)))
				{
					return false;
				}
			}
		}
		return true;
	}

	Matrix4& Matrix4::transposeSelf()
	{
		f32 temp = 0;
		Matrix4& self = *this;

		for (int32 i = 0; i < 4; i++)
		{
			for (int32 j = i + 1; j < 4; j++)
			{
				temp = self(i, j);
				self(i, j) = self(j, i);
				self(j, i) = temp;
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
	f32 Matrix4::minor(int32 row0, int32 row1, int32 row2, int32 col0, int32 col1, int32 col2) const
	{
		const Matrix4& self = *this;
		return self(row0, col0) * (self(row1, col1) * self(row2, col2) - self(row1, col2) * self(row2, col1)) -
			self(row0, col1) * (self(row1, col0) * self(row2, col2) - self(row1, col2) * self(row2, col0)) +
			self(row0, col2) * (self(row1, col0) * self(row2, col1) - self(row1, col1) * self(row2, col0));
	}

	// Calculate determinant recursively by getting minor for topmost row
	// We choose the topmost row to be omitted, calculate the 3x3 minor submatrix
	// for each element in the topmost row created by omitting the row and column that
	// the current element is in and sum up using the (-1 ^ i + j) pattern to create the
	// cofactors.
	f32 Matrix4::determinant() const
	{
		const Matrix4& self = *this;
		return self(0, 0) * minor(1, 2, 3, 1, 2, 3) -
			self(0, 1) * minor(1, 2, 3, 0, 2, 3) +
			self(0, 2) * minor(1, 2, 3, 0, 1, 3) -
			self(0, 3) * minor(1, 2, 3, 0, 1, 2);
	}

	// Classical adjoint32 is transpose of matrix of cofactors
	Matrix4 Matrix4::adjoint32() const
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
		return adjoint32() / det;
	}

	Matrix4& Matrix4::inverseSelf()
	{
		*this = inverse();
		return *this;
	}

	Matrix3 Matrix4::asMatrix3() const
	{
		const Matrix4& self = *this;

		return Matrix3{
			self(0,0), self(0,1), self(0,2),
			self(1,0), self(1,1), self(1,2),
			self(2,0), self(2,1), self(2,2)
		};
	}

	// You should only call this if the matrix contains only 
	// a rotation. Matrices containing other transforms
	// are not guaranteed to return the correct orientation.
	EulerAngles Matrix4::asEulerAngles() const
	{
		return asMatrix3().asEulerAngles();
	}


	Matrix4 Matrix4::scale(f32 x, f32 y, f32 z)
	{
		return Matrix4
		{
			x, 0, 0, 0,
			0, y, 0, 0,
			0, 0, z, 0,
			0, 0, 0, 1
		};
	}

	Matrix4 Matrix4::translation(f32 x, f32 y, f32 z)
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

	Matrix4 operator+(Matrix4 lhv, const Matrix4& rhv)
	{
		lhv += rhv;
		return lhv;
	}

	Matrix4 operator-(Matrix4 lhv, const Matrix4& rhv)
	{
		lhv -= rhv;
		return lhv;
	}

	Matrix4 operator*(Matrix4 lhv, const Matrix4& rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	Matrix4 operator+(Matrix4 lhv, f32 f)
	{
		lhv += f;
		return lhv;
	}

	Matrix4 operator-(Matrix4 lhv, f32 f)
	{
		lhv -= f;
		return lhv;
	}

	Matrix4 operator*(Matrix4 lhv, f32 f)
	{
		lhv *= f;
		return lhv;
	}

	Matrix4 operator/(Matrix4 lhv, f32 f)
	{
		lhv /= f;
		return lhv;
	}
}