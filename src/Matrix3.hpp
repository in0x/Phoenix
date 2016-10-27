#pragma once

#include <iostream>
#include "MatrixData.hpp"

namespace phoenix::math
{
	class quaternion;
	class vec3;

	class matrix3
	{
	private:
		matrixData<3> m_data;	
		float minor(int row0, int row1, int col0, int col1) const;
		matrix3 adjoint() const;

	public:
		matrix3()
			: m_data(matrixData<3>{})
		{}

		matrix3(const quaternion& rotation);

		matrix3(float m00, float m01, float m02,
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

		const float& operator()(std::size_t row, std::size_t col) const;

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

		matrix3& operator+=(const matrix3& rhv);
		matrix3& operator-=(const matrix3& rhv);
		matrix3& operator*=(const matrix3& rhv);

		vec3 operator*(vec3 rhv) const;

		matrix3& operator+=(float f);
		matrix3& operator-=(float f);
		matrix3& operator*=(float f);
		matrix3& operator/=(float f);

		bool operator==(const matrix3& rhv);

		float determinant() const;
		matrix3& transposeSelf();
		matrix3 transpose() const;
		matrix3& inverseSelf();
		matrix3 inverse() const;

		static matrix3 fromQuaternion(const quaternion& rotate);
		static matrix3 fromEulerAngles(float x, float y, float z);

		static matrix3 identity();
	};

	inline matrix3 operator+(matrix3 lhv, const matrix3& rhv)
	{
		lhv += rhv;
		return lhv;
	}

	inline matrix3 operator-(matrix3 lhv, const matrix3& rhv)
	{
		lhv -= rhv;
		return lhv;
	}

	inline matrix3 operator*(matrix3 lhv, const matrix3& rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	inline matrix3 operator+(matrix3 lhv, float f)
	{
		lhv += f;
		return lhv;
	}

	inline matrix3 operator-(matrix3 lhv, float f)
	{
		lhv -= f;
		return lhv;
	}

	inline matrix3 operator*(matrix3 lhv, float f)
	{
		lhv *= f;
		return lhv;
	}

	inline matrix3 operator/(matrix3 lhv, float f)
	{
		lhv /= f;
		return lhv;
	}

	inline std::ostream& operator<<(std::ostream& out, const matrix3& mat3)
	{
		std::cout << mat3(0, 0) << " " << mat3(0, 1) << " " << mat3(0, 2) << "\n"
			<< mat3(1, 0) << " " << mat3(1, 1) << " " << mat3(1, 2) << "\n"
			<< mat3(2, 0) << " " << mat3(2, 1) << " " << mat3(2, 2);
		return out;
	}
}