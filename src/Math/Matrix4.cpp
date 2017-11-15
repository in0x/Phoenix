#include "Matrix4.hpp"
#include "Matrix3.hpp"
#include "Vec4.hpp"
#include "Vec3.hpp"
#include "EulerAngles.hpp"
#include "PhiMath.hpp"
#include <assert.h>

namespace Phoenix
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

	float& Matrix4::operator()(int row, int col)
	{
		return m_data[col][row];
	}

	const float Matrix4::operator()(int row, int col) const
	{
		return m_data[col][row];
	}

	Matrix4& Matrix4::operator+=(const Matrix4& rhv)
	{
		Matrix4& lhv = *this;

		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				lhv(row, col) += rhv(row, col);
			}
		}

		return *this;
	}

	Matrix4& Matrix4::operator-=(const Matrix4& rhv)
	{
		Matrix4& lhv = *this;

		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
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

		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
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

		for (int i = 0; i < 4; i++)
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

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (!(lhv(i, j) == rhv(i, j)))
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

		for (int i = 0; i < 4; i++)
		{
			for (int j = i + 1; j < 4; j++)
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
	float Matrix4::minor(int row0, int row1, int row2, int col0, int col1, int col2) const
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
	float Matrix4::determinant() const
	{
		const Matrix4& self = *this;
		return self(0, 0) * minor(1, 2, 3, 1, 2, 3) -
			self(0, 1) * minor(1, 2, 3, 0, 2, 3) +
			self(0, 2) * minor(1, 2, 3, 0, 1, 3) -
			self(0, 3) * minor(1, 2, 3, 0, 1, 2);
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
			1, 0, 0, x,
			0, 1, 0, y,
			0, 0, 1, z,
			0, 0, 0, 1
		};
	}

	Matrix4 Matrix4::rotation(float x, float y, float z)
	{
		auto radX = radians(x);
		auto radY = radians(y);
		auto radZ = radians(z);

		float A = std::cos(radX);
		float B = std::sin(radX);
		float C = std::cos(radY);
		float D = std::sin(radY);
		float E = std::cos(radZ);
		float F = std::sin(radZ);

		return Matrix4
		{
			C*E,           -C*F,        -D,  0.f,
		   -B*D*E + A*F,  B*D*F + A*E, -B*C, 0.f,
			A*D*E + B*F, -A*D*F + B*E,  A*C, 0.f,
			0.f,		  0.f,			0.f, 1.f
		};
	}

	Matrix4 Matrix4::identity()
	{
		return Matrix4{ 1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1 };
	}

	Vec4 operator*(const Matrix4& lhv, const Vec4& rhv)
	{
		return Vec4(lhv(0, 0) * rhv.x + lhv(0, 1) * rhv.y + lhv(0, 2) * rhv.z + lhv(0, 3) * rhv.w,
					lhv(1, 0) * rhv.x + lhv(1, 1) * rhv.y + lhv(1, 2) * rhv.z + lhv(1, 3) * rhv.w,
					lhv(2, 0) * rhv.x + lhv(2, 1) * rhv.y + lhv(2, 2) * rhv.z + lhv(2, 3) * rhv.w,
					lhv(3, 0) * rhv.x + lhv(3, 1) * rhv.y + lhv(3, 2) * rhv.z + lhv(3, 3) * rhv.w);
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

	Matrix4 operator+(Matrix4 lhv, float f)
	{
		lhv += f;
		return lhv;
	}

	Matrix4 operator-(Matrix4 lhv, float f)
	{
		lhv -= f;
		return lhv;
	}

	Matrix4 operator*(Matrix4 lhv, float f)
	{
		lhv *= f;
		return lhv;
	}

	Matrix4 operator/(Matrix4 lhv, float f)
	{
		lhv /= f;
		return lhv;
	}
}