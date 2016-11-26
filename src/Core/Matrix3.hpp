#pragma once

#include <iostream>
#include "MatrixData.hpp"
#include "PhiCoreRequired.hpp"

namespace Phoenix
{
	class Matrix4;
	class Quaternion;
	class Vec3;
	class EulerAngles;

	class Matrix3
	{
	public:
		MatrixData<3> m_data;

		Matrix3()
			: m_data(MatrixData<3>{})
		{}

		Matrix3(f32 m00, f32 m01, f32 m02,
				f32 m10, f32 m11, f32 m12, 
				f32 m20, f32 m21, f32 m22);

		f32& operator()(int32 row, int32 col);

		const f32 operator()(int32 row, int32 col) const;

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

		Matrix3& operator+=(f32 f);
		Matrix3& operator-=(f32 f);
		Matrix3& operator*=(f32 f);
		Matrix3& operator/=(f32 f);

		bool operator==(const Matrix3& rhv);

		f32 determinant() const;
		f32 minor(int32 row0, int32 row1, int32 col0, int32 col1) const;
		Matrix3 adjoint32() const;

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

	Matrix3 operator+(Matrix3 lhv, const Matrix3& rhv);
	Matrix3 operator-(Matrix3 lhv, const Matrix3& rhv);
	Matrix3 operator*(Matrix3 lhv, const Matrix3& rhv);
	Matrix3 operator+(Matrix3 lhv, f32 f);
	Matrix3 operator-(Matrix3 lhv, f32 f);
	Matrix3 operator*(Matrix3 lhv, f32 f);
	Matrix3 operator/(Matrix3 lhv, f32 f);

	inline std::ostream& operator<<(std::ostream& out, const Matrix3& mat3)
	{
		std::cout << mat3(0, 0) << " " << mat3(0, 1) << " " << mat3(0, 2) << "\n"
			<< mat3(1, 0) << " " << mat3(1, 1) << " " << mat3(1, 2) << "\n"
			<< mat3(2, 0) << " " << mat3(2, 1) << " " << mat3(2, 2);
		return out;
	}
}