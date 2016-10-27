#pragma once

#include <iostream>
#include "MatrixData.hpp"

namespace phoenix::math
{
	class quaternion;
	class Vec4;
	class vec3;

	// Column-major 4x4 matrix.
	class matrix4
	{
	private:
		matrixData<4> m_data;

		float minor(int row0, int row1, int row2, int col0, int col1, int col2) const;
		matrix4 adjoint() const;

	public:
		matrix4()
			: m_data(matrixData<4>{})
		{}

		matrix4(float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33);

		// Returns the number of columns in this matrix.
		// Useful for porting matrix-vector interactions
		// easily between different sizes.
		std::size_t columns() const
		{
			return m_data.size();
		}

		// Returns the number of rows in this matrix.
		std::size_t rows() const
		{
			return m_data[0].size();
		}

		// Subscript operator
		// Sets out of bounds indices to 0. Accesing (4,2)
		// would for example return (0,2).
		float& operator()(std::size_t row, std::size_t col);

		// Subscript operator
		// Sets out of bounds indices to 0. Accesing (4,2)
		// would for example return (0,2).
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

		matrix4& operator+=(const matrix4& rhv);
		matrix4& operator-=(const matrix4& rhv);
		matrix4& operator*=(const matrix4& rhv);
		
		Vec4 operator*(Vec4 rhv) const;
		vec3 operator*(vec3 rhv) const;

		matrix4& operator+=(float f);
		matrix4& operator-=(float f);
		matrix4& operator*=(float f);
		matrix4& operator/=(float f);

		bool operator==(const matrix4& rhv);

		float determinant() const;
		matrix4& transposeSelf();
		matrix4 transpose() const;
		matrix4& inverseSelf();
		matrix4 inverse() const;

		static matrix4 scale(float x, float y, float z);
		static matrix4 translation(float x, float y, float z);
		static matrix4 rotation(const quaternion& rotate);

		static matrix4 identity();
	};

	inline matrix4 operator+(matrix4 lhv, const matrix4& rhv)
	{
		lhv += rhv;
		return lhv;
	}

	inline matrix4 operator-(matrix4 lhv, const matrix4& rhv)
	{
		lhv -= rhv;
		return lhv;
	}

	inline matrix4 operator*(matrix4 lhv, const matrix4& rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	inline matrix4 operator+(matrix4 lhv, float f)
	{
		lhv += f;
		return lhv;
	}

	inline matrix4 operator-(matrix4 lhv, float f)
	{
		lhv -= f;
		return lhv;
	}

	inline matrix4 operator*(matrix4 lhv, float f)
	{
		lhv *= f;
		return lhv;
	}

	inline matrix4 operator/(matrix4 lhv, float f)
	{
		lhv /= f;
		return lhv;
	}

	inline std::ostream& operator<<(std::ostream& out, const matrix4& mat4)
	{
		std::cout << mat4(0, 0) << " " << mat4(0, 1) << " " << mat4(0, 2) << " " << mat4(0, 3) << "\n"
			  	  << mat4(1, 0) << " " << mat4(1, 1) << " " << mat4(1, 2) << " " << mat4(1, 3) << "\n"
				  << mat4(2, 0) << " " << mat4(2, 1) << " " << mat4(2, 2) << " " << mat4(2, 3) << "\n"
				  << mat4(3, 0) << " " << mat4(3, 1) << " " << mat4(3, 2) << " " << mat4(3, 3);
		return out;
	}
}