#pragma once

#include "MatrixData.hpp"

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

		Matrix3() = default;

		Matrix3(float m00, float m01, float m02,
			float m10, float m11, float m12,
			float m20, float m21, float m22);

		float& operator()(int row, int col);

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
		float minor(int row0, int row1, int col0, int col1) const;
		Matrix3 adjoint() const;

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
	Matrix3 operator+(Matrix3 lhv, float f);
	Matrix3 operator-(Matrix3 lhv, float f);
	Matrix3 operator*(Matrix3 lhv, float f);
	Matrix3 operator/(Matrix3 lhv, float f);
}