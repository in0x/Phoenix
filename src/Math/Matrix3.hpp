#pragma once

#include <iostream>
#include "MatrixData.hpp"

namespace Phoenix
{
	class Matrix4;
	class Quaternion;
	class Vec3;
	class EulerAngles;

	class Matrix3
	{
	private:
		MatrixData<3> m_data;	
		float minor(int row0, int row1, int col0, int col1) const;
		Matrix3 adjoint() const;

	public:
		Matrix3()
			: m_data(MatrixData<3>{})
		{}

		Matrix3(float m00, float m01, float m02,
				float m10, float m11, float m12, 
				float m20, float m21, float m22);

		std::size_t columns() const
		{
			return m_data.size();
		}

		std::size_t rows() const
		{
			return m_data[0].size();
		}

		float& operator()(std::size_t row, std::size_t col);

		const float operator()(std::size_t row, std::size_t col) const;

		decltype(auto) begin()
		{
			return m_data.begin();
		}

		decltype(auto) end()
		{
			return m_data.end();
		}

		decltype(auto) begin() const
		{
			return m_data.begin();
		}

		decltype(auto) end() const
		{
			return m_data.end();
		}

		Matrix3& operator+=(const Matrix3& rhv);
		Matrix3& operator-=(const Matrix3& rhv);
		Matrix3& operator*=(const Matrix3& rhv);

		Vec3 operator*(Vec3 rhv) const;

		Matrix3& operator+=(float f);
		Matrix3& operator-=(float f);
		Matrix3& operator*=(float f);
		Matrix3& operator/=(float f);

		bool operator==(const Matrix3& rhv);

		float determinant() const;
		Matrix3& transposeSelf();
		Matrix3 transpose() const;
		Matrix3& inverseSelf();
		Matrix3 inverse() const;

		Matrix4 asMatrix4() const;
		EulerAngles asEulerAngles() const;

		static Matrix3 fromEulerAngles(const EulerAngles& angles);
		static Matrix3 fromQuaternion(const Quaternion& quat);

		static Matrix3 identity();
	};

	inline Matrix3 operator+(Matrix3 lhv, const Matrix3& rhv)
	{
		lhv += rhv;
		return lhv;
	}

	inline Matrix3 operator-(Matrix3 lhv, const Matrix3& rhv)
	{
		lhv -= rhv;
		return lhv;
	}

	inline Matrix3 operator*(Matrix3 lhv, const Matrix3& rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	inline Matrix3 operator+(Matrix3 lhv, float f)
	{
		lhv += f;
		return lhv;
	}

	inline Matrix3 operator-(Matrix3 lhv, float f)
	{
		lhv -= f;
		return lhv;
	}

	inline Matrix3 operator*(Matrix3 lhv, float f)
	{
		lhv *= f;
		return lhv;
	}

	inline Matrix3 operator/(Matrix3 lhv, float f)
	{
		lhv /= f;
		return lhv;
	}

	inline std::ostream& operator<<(std::ostream& out, const Matrix3& mat3)
	{
		std::cout << mat3(0, 0) << " " << mat3(0, 1) << " " << mat3(0, 2) << "\n"
			<< mat3(1, 0) << " " << mat3(1, 1) << " " << mat3(1, 2) << "\n"
			<< mat3(2, 0) << " " << mat3(2, 1) << " " << mat3(2, 2);
		return out;
	}
}