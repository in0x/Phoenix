#pragma once

#include "PhiCoreRequired.hpp"
#include <iostream>
#include "MatrixData.hpp"

namespace Phoenix {

	class Quaternion;
	class Matrix3;
	class Vec4;
	class Vec3;
	class EulerAngles;

	// Column-major 4x4 matrix.
	class Matrix4
	{
	public:
		MatrixData<4> m_data;

		Matrix4()
			: m_data(MatrixData<4>{})
		{}

		Matrix4(f32 m00, f32 m01, f32 m02, f32 m03,
			f32 m10, f32 m11, f32 m12, f32 m13,
			f32 m20, f32 m21, f32 m22, f32 m23,
			f32 m30, f32 m31, f32 m32, f32 m33);

		// Subscript operator
		f32& operator()(int32 row, int32 col);

		// Subscript operator
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

		Matrix4& operator+=(const Matrix4& rhv);
		Matrix4& operator-=(const Matrix4& rhv);
		Matrix4& operator*=(const Matrix4& rhv);

		Vec4 operator*(Vec4 rhv) const;
		Vec3 operator*(Vec3 rhv) const;

		Matrix4& operator+=(f32 f);
		Matrix4& operator-=(f32 f);
		Matrix4& operator*=(f32 f);
		Matrix4& operator/=(f32 f);

		bool operator==(const Matrix4& rhv);

		f32 determinant() const;
		f32 minor(int32 row0, int32 row1, int32 row2, int32 col0, int32 col1, int32 col2) const;
		Matrix4 adjoint32() const;

		Matrix4& transposeSelf();
		Matrix4 transpose() const;

		Matrix4& inverseSelf();
		Matrix4 inverse() const;

		Matrix3 asMatrix3() const;
		EulerAngles asEulerAngles() const;

		static Matrix4 scale(f32 x, f32 y, f32 z);
		static Matrix4 translation(f32 x, f32 y, f32 z);

		static Matrix4 identity();
	};

	Matrix4 operator+(Matrix4 lhv, const Matrix4& rhv);
	Matrix4 operator-(Matrix4 lhv, const Matrix4& rhv);
	Matrix4 operator*(Matrix4 lhv, const Matrix4& rhv);
	Matrix4 operator+(Matrix4 lhv, f32 f);
	Matrix4 operator-(Matrix4 lhv, f32 f);
	Matrix4 operator*(Matrix4 lhv, f32 f);
	Matrix4 operator/(Matrix4 lhv, f32 f);

	inline std::ostream& operator<<(std::ostream& out, const Matrix4& mat4)
	{
		std::cout << mat4(0, 0) << " " << mat4(0, 1) << " " << mat4(0, 2) << " " << mat4(0, 3) << "\n"
			<< mat4(1, 0) << " " << mat4(1, 1) << " " << mat4(1, 2) << " " << mat4(1, 3) << "\n"
			<< mat4(2, 0) << " " << mat4(2, 1) << " " << mat4(2, 2) << " " << mat4(2, 3) << "\n"
			<< mat4(3, 0) << " " << mat4(3, 1) << " " << mat4(3, 2) << " " << mat4(3, 3);
		return out;
	}
}