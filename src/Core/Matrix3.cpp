#include "Matrix3.hpp"
#include "Common.hpp"
#include "Matrix4.hpp"
#include "Vec3.hpp"
#include "EulerAngles.hpp"
#include "Quaternion.hpp"

namespace Phoenix
{
	Matrix3::Matrix3(f32 m00, f32 m01, f32 m02, 
					 f32 m10, f32 m11, f32 m12,
					 f32 m20, f32 m21, f32 m22)
	{
		m_data[0][0] = m00; m_data[0][1] = m10; m_data[0][2] = m20;
		m_data[1][0] = m01; m_data[1][1] = m11; m_data[1][2] = m21;
		m_data[2][0] = m02; m_data[2][1] = m12; m_data[2][2] = m22;
	}

	f32& Matrix3::operator()(int32 row, int32 col)
	{
		return m_data[col][row];
	}

	const f32 Matrix3::operator()(int32 row, int32 col) const
	{
		return m_data[col][row];
	}

	Matrix3& Matrix3::operator+=(const Matrix3& rhv)
	{
		Matrix3& lhv = *this;

		for (int32 row = 0; row < 3; row++)
		{
			for (int32 col = 0; col < 3; col++)
			{
				lhv(row, col) += rhv(row, col);
			}
		}

		return *this;
	}

	Matrix3& Matrix3::operator-=(const Matrix3& rhv)
	{
		Matrix3& lhv = *this;

		for (int32 row = 0; row < 3; row++)
		{
			for (int32 col = 0; col < 3; col++)
			{
				lhv(row, col) -= rhv(row, col);
			}
		}

		return *this;
	}

	Matrix3& Matrix3::operator*=(const Matrix3& rhv)
	{
		Matrix3 dest{};
		Matrix3& lhv = *this;

		for (int32 row = 0; row < 3; row++)
		{
			for (int32 col = 0; col < 3; col++)
			{
				dest(row, col) = lhv(row, 0) * rhv(0, col) + lhv(row, 1) * rhv(1, col) + lhv(row, 2) * rhv(2, col);
			}
		}

		*this = dest;
		return *this;
	}

	Vec3 Matrix3::operator*(Vec3 rhv) const
	{
		Vec3 dest{};
		const Matrix3& lhv = *this;

		for (int32 i = 0; i < 3; i++)
		{
			dest(i) += lhv(i, 0) * rhv(0) + lhv(i, 1) * rhv(1) + lhv(i, 2) * rhv(2);
		}

		return dest;
	}

	Matrix3& Matrix3::operator+=(f32 f)
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

	Matrix3& Matrix3::operator-=(f32 f)
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

	Matrix3& Matrix3::operator*=(f32 f)
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

	Matrix3& Matrix3::operator/=(f32 f)
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

	bool Matrix3::operator==(const Matrix3& rhv)
	{
		Matrix3& lhv = *this;
		
		for (int32 i = 0; i < 3; i++)
		{
			for (int32 j = 0; j < 3; j++)
			{
				if (!almostEqualRelative(lhv(i, j), rhv(i, j)))
				{
					return false;
				}
			}
		}
		return true;
	}

	Matrix3& Matrix3::transposeSelf()
	{
		f32 temp = 0;
		Matrix3& self = *this;
		for (int32 i = 0; i < 3; i++)
		{
			for (int32 j = i + 1; j < 3; j++)
			{
				temp = self(i, j);
				self(i, j) = self(j, i);
				self(j, i) = temp;
			}
		}
		return *this;
	}

	Matrix3 Matrix3::transpose() const
	{
		Matrix3 transp = *this;
		transp.transposeSelf();
		return transp;
	}

	// Calculate minor from 2x2 sub matrix 
	// Passing the indices of the 2 rows and columns which make up the minor matrix allows us
	// to easily calculate the minor without having to figure out the omitted row and column
	// ourselfs.
	f32 Matrix3::minor(int32 row0, int32 row1, int32 col0, int32 col1) const
	{
		const Matrix3& self = *this;
		return self(row0, col0) * self(row1, col1) - self(row1, col0) * self(row0, col1);
	}

	// Calculate determinant recursively by getting minor for topmost row
	// We choose the topmost row to be omitted, calculate the 2x2 minor submatrix
	// for each element in the topmost row created by omitting the row and column that
	// the current element is in and sum up using the (-1 ^ i + j) pattern to create the
	// cofactors.
	f32 Matrix3::determinant() const
	{
		const Matrix3& self = *this;
		return self(0, 0) * minor(1, 2, 1, 2) -
			   self(0, 1) * minor(1, 2, 0, 2) +
			   self(0, 2) * minor(1, 2, 0, 1);
	}

	// Classical adjoint32 is transpose of matrix of cofactors
	Matrix3 Matrix3::adjoint() const
	{
		return Matrix3
		{
			minor(1,2,1,2), -minor(0,2,1,2),  minor(0,1,1,2),
		   -minor(1,2,0,2),  minor(0,2,0,2), -minor(0,1,0,2),
			minor(1,2,0,1), -minor(0,2,0,1),  minor(0,1,0,1)
		};
	}

	Matrix3 Matrix3::inverse() const
	{
		auto det = determinant();
		assert(det);
		return adjoint() / det;
	}

	Matrix3& Matrix3::inverseSelf()
	{
		*this = inverse();
		return *this;
	}

	Matrix4 Matrix3::asMatrix4() const
	{
		const Matrix3& self = *this;
		return Matrix4 {
			self(0,0), self(0,1), self(0,2), 0,
			self(1,0), self(1,1), self(1,2), 0,
			self(2,0), self(2,1), self(2,2), 0,
			0,		   0,		  0,		 1
		};
	}

	// You should only call this if the matrix contains only 
	// a rotation. Matrices containing other transforms
	// are not guaranteed to return the correct orientation.
	EulerAngles Matrix3::asEulerAngles() const
	{
		f32 x, y, z;
		const Matrix3& self = *this;

		y = -std::asin(self(0, 2));
		auto C = std::cos(y);

		if (std::abs(C) > 0.005f) // Account for f32ing point32 inaccuracy
		{
			auto A = self(2, 2) / C;
			auto B = -self(1, 2) / C;

			// tanx = sinx / cosx
			x = std::atan2(B, A);

			auto E = self(0, 0) / C;
			auto F = -self(0, 1) / C;

			z = std::atan2(F, E);
		}
		else // Gimball Lock
		{
			x = 0;

			// D is 1
			auto BEAF = self(1, 1);
			auto AEBF = self(1, 0);

			z = std::atan2(AEBF, BEAF);
		}

		return EulerAngles{ degrees(x), degrees(y), degrees(z) };
	}

	// Basically, we directly concatenate the 3 matrices for
	// yaw, pitch and roll, saving us a lot of superflous operations
	// that 3 seperate matrices would entail.
	Matrix3 Matrix3::fromEulerAngles(const EulerAngles& angles)
	{
		auto x = radians(angles.x);
		auto y = radians(angles.y);
		auto z = radians(angles.z);

		f32 A = std::cos(x);
		f32 B = std::sin(x);
		f32 C = std::cos(y);
		f32 D = std::sin(y);
		f32 E = std::cos(z);
		f32 F = std::sin(z);

		return Matrix3
		{
			C*E,         -C*F,         -D,
			-B*D*E + A*F,  B*D*F + A*E, -B*C,
			A*D*E + B*F, -A*D*F + B*E,  A*C
		};
	}

	Matrix3 Matrix3::fromQuaternion(const Quaternion& quat)
	{
		return Matrix3{
			1 - 2 * quat.y*quat.y - 2 * quat.z*quat.z, 2 * quat.x*quat.y + 2 * quat.w*quat.z, 2 * quat.x*quat.z - 2 * quat.w*quat.y,
			2 * quat.x*quat.y - 2 * quat.w*quat.z, 1 - 2 * quat.x*quat.x - 2 * quat.z*quat.z, 2 * quat.y*quat.z + 2 * quat.w*quat.x,
			2 * quat.x*quat.z + 2 * quat.w*quat.y, 2 * quat.y*quat.z - 2 * quat.w*quat.x, 1 - 2 * quat.x*quat.x - 2 * quat.y*quat.y
		};
	}

	Matrix3 Matrix3::identity()
	{
		return Matrix3{ 1, 0, 0,
						0, 1, 0,
						0, 0, 1 };
	}

	Vec3 operator*(const Matrix3& lhv, Vec3 rhv)
	{
		return rhv;
	}

	Matrix3 operator+(Matrix3 lhv, const Matrix3& rhv)
	{
		lhv += rhv;
		return lhv;
	}

	Matrix3 operator-(Matrix3 lhv, const Matrix3& rhv)
	{
		lhv -= rhv;
		return lhv;
	}

	Matrix3 operator*(Matrix3 lhv, const Matrix3& rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	Matrix3 operator+(Matrix3 lhv, f32 f)
	{
		lhv += f;
		return lhv;
	}

	Matrix3 operator-(Matrix3 lhv, f32 f)
	{
		lhv -= f;
		return lhv;
	}

	Matrix3 operator*(Matrix3 lhv, f32 f)
	{
		lhv *= f;
		return lhv;
	}

	Matrix3 operator/(Matrix3 lhv, f32 f)
	{
		lhv /= f;
		return lhv;
	}
}