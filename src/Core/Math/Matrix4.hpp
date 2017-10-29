#pragma once

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

		constexpr Matrix4()
			: m_data(MatrixData<4>())
		{}

		Matrix4(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);

		// Subscript operator
		float& operator()(int row, int col);

		// Subscript operator
		const float operator()(int row, int col) const;

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

		Matrix4& operator+=(float f);
		Matrix4& operator-=(float f);
		Matrix4& operator*=(float f);
		Matrix4& operator/=(float f);

		bool operator==(const Matrix4& rhv);

		float determinant() const;
		float minor(int row0, int row1, int row2, int col0, int col1, int col2) const;
		Matrix4 adjoint() const;

		Matrix4& transposeSelf();
		Matrix4 transpose() const;

		Matrix4& inverseSelf();
		Matrix4 inverse() const;

		Matrix3 asMatrix3() const;
		EulerAngles asEulerAngles() const;

		static Matrix4 scale(float x, float y, float z);
		static Matrix4 translation(float x, float y, float z);
		static Matrix4 rotation(float x, float y, float z);

		static Matrix4 identity();
	};

	Matrix4 operator+(Matrix4 lhv, const Matrix4& rhv);
	Matrix4 operator-(Matrix4 lhv, const Matrix4& rhv);
	Matrix4 operator*(Matrix4 lhv, const Matrix4& rhv);
	Matrix4 operator+(Matrix4 lhv, float f);
	Matrix4 operator-(Matrix4 lhv, float f);
	Matrix4 operator*(Matrix4 lhv, float f);
	Matrix4 operator/(Matrix4 lhv, float f);
}