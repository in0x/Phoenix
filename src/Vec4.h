#pragma once
#include <iostream>
#include <cmath>
#include <cassert>

namespace Phoenix::Math
{
	class Vec4
	{
	public:
		float x, y, z, w;

		Vec4()
			: x(0.f)
			, y(0.f)
			, z(0.f)
			, w(0.f)
		{}

		Vec4(float f)
			: x(f)
			, y(f)
			, z(f)
			, w(f)
		{}

		Vec4(float _x, float _y, float _z, float _w)
			: x(_x)
			, y(_y)
			, z(_z)
			, w (_w)
		{}

		float& operator[] (int i)
		{
			return *(&x + i);
		}

		const float& operator[](int i) const
		{
			return *(&x + i);
		}

		Vec4& operator+=(const Vec4& rhv)
		{
			x += rhv.x;
			y += rhv.y;
			z += rhv.z;
			w += rhv.w;
			return *this;
		}

		Vec4& operator-=(const Vec4& rhv)
		{
			x -= rhv.x;
			y -= rhv.y;
			z -= rhv.z;
			w -= rhv.w;
			return *this;
		}

		Vec4& operator*=(const Vec4& rhv)
		{
			x *= rhv.x;
			y *= rhv.y;
			z *= rhv.z;
			w *= rhv.w;
			return *this;
		}

		Vec4& operator/=(const Vec4& rhv)
		{
			x /= rhv.x;
			y /= rhv.y;
			z /= rhv.z;
			w /= rhv.w;
			return *this;
		}

		Vec4& operator+=(float rhv)
		{
			x += rhv;
			y += rhv;
			z += rhv;
			w += rhv;
			return *this;
		}

		Vec4& operator-=(float rhv)
		{
			x -= rhv;
			y -= rhv;
			z -= rhv;
			w -= rhv;
			return *this;
		}

		Vec4& operator*=(float rhv)
		{
			x *= rhv;
			y *= rhv;
			z *= rhv;
			w *= rhv;
			return *this;
		}

		Vec4& operator/=(float rhv)
		{
			assert(rhv != 0);
			x /= rhv;
			y /= rhv;
			z /= rhv;
			w /= rhv;
			return *this;
		}

		bool operator==(const Vec4& rhv) const
		{
			return x == rhv.x && y == rhv.y && z == rhv.z && w == rhv.w;
		}

		float length() const
		{
			return std::sqrt(x * x + y * y + z * z + w * w);
		}

		float length2() const
		{
			return x * x + y * y + z * z + w * w;
		}

		float distance(const Vec4& rhv) const
		{
			return std::sqrt((x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y) - (z * rhv.z) * (z * rhv.z) - (w * rhv.w) * (w * rhv.w));
		}

		float distance2(const Vec4& rhv) const
		{
			return (x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y) - (z * rhv.z) * (z * rhv.z) - (w * rhv.w) * (w * rhv.w);
		}

		Vec4& normalize()
		{
			float len = length();
			x /= len;
			y /= len;
			z /= len;
			w /= len;
			return *this;
		}

		float dot(const Vec4& rhv) const
		{
			return x * rhv.x + y * rhv.y + z * rhv.z + w * rhv.w; 
		}

		Vec4 reflect(const Vec4& rhv) const;
	};

	inline const Vec4 operator+(Vec4 lhv, const Vec4& rhv)
	{
		lhv += rhv;
		return lhv;
	}

	inline const Vec4 operator-(Vec4 lhv, const Vec4& rhv)
	{
		lhv -= rhv;
		return lhv;
	}

	inline const Vec4 operator*(Vec4 lhv, const Vec4& rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	inline const Vec4 operator/(Vec4 lhv, const Vec4& rhv)
	{
		lhv /= rhv;
		return lhv;
	}

	inline const Vec4 operator+(const Vec4& lhv, float rhv)
	{
		return Vec4{ lhv.x + rhv, lhv.y + rhv, lhv.z + rhv, lhv.w + rhv };
	}

	inline const Vec4 operator-(const Vec4& lhv, float rhv)
	{
		return Vec4{ lhv.x - rhv, lhv.y - rhv, lhv.z - rhv, lhv.w - rhv };
	}

	inline const Vec4 operator*(const Vec4& lhv, float rhv)
	{
		return Vec4{ lhv.x * rhv, lhv.y * rhv, lhv.z * rhv, lhv.w * rhv };
	}

	inline const Vec4 operator/(const Vec4& lhv, float rhv)
	{
		return Vec4{ lhv.x / rhv, lhv.y / rhv, lhv.z / rhv, lhv.w / rhv };
	}

	inline Vec4 Vec4::reflect(const Vec4& rhv) const
	{
		return 2.f * rhv.dot(*this) * (*this) - rhv;
	}

	inline std::ostream& operator<<(std::ostream& out, const Phoenix::Vec4& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << vec.z << " | " << vec.w << " )";
		return out;
	}
}

