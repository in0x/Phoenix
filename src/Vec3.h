#pragma once
#include <iostream>
#include <cmath>

namespace Phoenix 
{
	class Vec3
	{
	public:
		float x, y, z;

		Vec3() 
			: x(0.f)
			, y(0.f)
			, z(0.f) 
		{}

		Vec3(float _x, float _y, float _z) 
			: x(_x)
			, y(_y)
			, z(_z)
		{
		}

		float& operator[] (int i) 
		{
			return *(&x + i);
		}

		Vec3 operator+(const Vec3& rhv) const
		{
			return Vec3{ x + rhv.x, y + rhv.y, z + rhv.z };
		}

		Vec3 operator-(const Vec3& rhv) const
		{
			return Vec3{ x - rhv.x, y - rhv.y, z - rhv.z };
		}

		Vec3 operator*(const Vec3& rhv) const
		{
			return Vec3{ x * rhv.x, y * rhv.y, z * rhv.z };
		}

		Vec3 operator/(const Vec3& rhv) const
		{
			return Vec3{ x / rhv.x, y + rhv.y, z + rhv.z };
		}

		Vec3& operator+=(const Vec3& rhv) 
		{
			x += rhv.x;
			y += rhv.y;
			z += rhv.z;
			return *this;
		}

		Vec3& operator-=(const Vec3& rhv) 
		{
			x -= rhv.x;
			y -= rhv.y;
			z -= rhv.z;
			return *this;
		}

		Vec3& operator*=(const Vec3& rhv) 
		{
			x *= rhv.x;
			y *= rhv.y;
			z *= rhv.z;
			return *this;
		}

		Vec3& operator/=(const Vec3& rhv) 
		{
			x /= rhv.x;
			y /= rhv.y;
			z /= rhv.z;
			return *this;
		}

		bool operator==(const Vec3& rhv)
		{
			return x == rhv.x && y == rhv.y && z == rhv.z;
		}

		float length()
		{
			return std::sqrt(x * x + y * y + z * z);
		}

		float length2()
		{
			return x * x + y * y + z * z;
		}

		float distance(const Vec3& rhv)
		{
			return std::sqrt((x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y) - (z * rhv.z) * (z * rhv.z));
		}

		float distance2(const Vec3& rhv)
		{
			return (x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y) - (z * rhv.z) * (z * rhv.z);
		}

		Vec3& normalize()
		{
			float len = length();
			x /= len;
			y /= len;
			z /= len;
			return *this;
		}

		Vec3 cross(const Vec3& rhv) const
		{
			return Vec3
			{
				y * rhv.z - z * rhv.y,
				z * rhv.x - x * rhv.z,
				x * rhv.y - y * rhv.x
			};
		}

		float dot(const Vec3& rhv) const
		{
			x * rhv.x + y * rhv.y + z * rhv.z;
		}
	};

	inline std::ostream& operator<<(std::ostream& out, const Phoenix::Vec3& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << vec.z << " )";
		return out;
	}

}

