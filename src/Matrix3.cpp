#include "Matrix3.hpp"

#include "Vec3.hpp"

namespace phoenix::math
{
	matrix3::matrix3(float m00, float m01, float m02, 
					 float m10, float m11, float m12,
					 float m20, float m21, float m22)
	{
		m_data[0][0] = m00; m_data[0][1] = m10; m_data[0][2] = m20;
		m_data[1][0] = m01; m_data[1][1] = m11; m_data[1][2] = m21;
		m_data[2][0] = m02; m_data[2][1] = m12; m_data[2][2] = m22;
	}

	matrix3::matrix3(const quaternion& rotation)
	{
	}

	float& matrix3::operator()(std::size_t row, std::size_t col)
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

	const float& matrix3::operator()(std::size_t row, std::size_t col) const
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

	matrix3& matrix3::operator+=(const matrix3& rhv)
	{
		matrix3& lhv = *this;

		for (std::size_t row = 0; row < rows(); row++)
		{
			for (std::size_t col = 0; col < columns(); col++)
			{
				lhv(row, col) += rhv(row, col);
			}
		}

		return *this;
	}

	matrix3& matrix3::operator-=(const matrix3& rhv)
	{
		matrix3& lhv = *this;

		for (std::size_t row = 0; row < rows(); row++)
		{
			for (std::size_t col = 0; col < columns(); col++)
			{
				lhv(row, col) -= rhv(row, col);
			}
		}

		return *this;
	}

	matrix3& matrix3::operator*=(const matrix3& rhv)
	{
		matrix3 dest{};
		matrix3& lhv = *this;

		for (int i = 0; i <= m_data.size(); i++)
		{
			for (int j = 0; j <= m_data.size(); j++)
			{
				dest(i, j) = lhv(i, 0) * rhv(0, j) + lhv(i, 1) * rhv(1, j) + lhv(i, 2) * rhv(2, j);
			}
		}

		*this = dest;
		return *this;
	}

	vec3 matrix3::operator*(vec3 rhv) const
	{
		vec3 dest{};
		const matrix3& lhv = *this;

		for (int i = 0; i < lhv.rows(); i++)
		{
			dest(i) += lhv(i, 0) * rhv(0) + lhv(i, 1) * rhv(1) + lhv(i, 2) * rhv(2);
		}

		return dest;
	}

	matrix3& matrix3::operator+=(float f)
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

	matrix3& matrix3::operator-=(float f)
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

	matrix3& matrix3::operator*=(float f)
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

	matrix3& matrix3::operator/=(float f)
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

	bool matrix3::operator==(const matrix3& rhv)
	{
		matrix3& lhv = *this;
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

	matrix3& matrix3::transposeSelf()
	{
		float temp = 0;
		matrix3& self = *this;
		for (int i = 0; i < rows(); i++)
		{
			for (int j = i + 1; j < columns(); j++)
			{
				temp = self(i, j);
				self(i, j) = self(j, i);
				self(j, i) = temp;
			}
		}
		return *this;
	}

	matrix3 matrix3::transpose() const
	{
		matrix3 transp = *this;
		transp.transposeSelf();
		return transp;
	}

	// Calculate minor from 2x2 sub matrix 
	// Passing the indices of the 2 rows and columns which make up the minor matrix allows us
	// to easily calculate the minor without having to figure out the omitted row and column
	// ourselfs.
	float matrix3::minor(int row0, int row1, int col0, int col1) const
	{
		const matrix3& self = *this;
		return self(row0, col0) * self(row1, col1) - self(row1, col0) * self(row0, col1);
	}

	// Calculate determinant recursively by getting minor for topmost row
	// We choose the topmost row to be omitted, calculate the 2x2 minor submatrix
	// for each element in the topmost row created by omitting the row and column that
	// the current element is in and sum up using the (-1 ^ i + j) pattern to create the
	// cofactors.
	float matrix3::determinant() const
	{
		const matrix3& self = *this;
		return self(0, 0) * minor(1, 2, 1, 2) -
			   self(0, 1) * minor(1, 2, 0, 2) +
			   self(0, 2) * minor(1, 2, 0, 1);
	}

	// Classical adjoint is transpose of matrix of cofactors
	matrix3 matrix3::adjoint() const
	{
		return matrix3
		{
			minor(1,2,1,2), -minor(0,2,1,2),  minor(0,1,1,2),
		   -minor(1,2,0,2),  minor(0,2,0,2), -minor(0,1,0,2),
			minor(1,2,0,1), -minor(0,2,0,1),  minor(0,1,0,1)
		};
	}

	matrix3 matrix3::inverse() const
	{
		auto det = determinant();
		assert(det);
		return adjoint() / det;
	}

	matrix3& matrix3::inverseSelf()
	{
		*this = inverse();
		return *this;
	}

	matrix3 matrix3::fromQuaternion(const quaternion& rotate)
	{
		return matrix3{};
	}

	matrix3 matrix3::fromEulerAngles(float x, float y, float z)
	{
		return matrix3{};
	}

	matrix3 matrix3::identity()
	{
		return matrix3{ 1, 0, 0,
						0, 1, 0,
						0, 0, 1 };
	}

	vec3 operator*(const matrix3& lhv, vec3 rhv)
	{
		return rhv;
	}
}