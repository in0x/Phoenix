#pragma once
#include <iostream>
#include <cmath>
#include <cassert>

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

		Vec3(float f) 
			: x(f)
			, y(f)
			, z(f)
		{}

		Vec3(float _x, float _y, float _z) 
			: x(_x)
			, y(_y)
			, z(_z)
		{}

		// Raw pointer arithmetic is against the Core Guidelines, but this is really neat.
		// I should figure out a way to do this more safely. This is also relies on int being 32-bit.
		float& operator[] (int i)  
		{
			return *(&x + i);
		}

		const float& operator[](int i) const
		{
			return *(&x + i);
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

		Vec3& operator+=(float rhv)
		{
			x += rhv;
			y += rhv;
			z += rhv;
			return *this;
		}

		Vec3& operator-=(float rhv)
		{
			x -= rhv;
			y -= rhv;
			z -= rhv;
			return *this;
		}

		Vec3& operator*=(float rhv)
		{
			x *= rhv;
			y *= rhv;
			z *= rhv;
			return *this;
		}

		Vec3& operator/=(float rhv)
		{
			assert(rhv != 0);
			x /= rhv;
			y /= rhv;
			z /= rhv;
			return *this;
		}

		bool operator==(const Vec3& rhv) const
		{
			return x == rhv.x && y == rhv.y && z == rhv.z;
		}

		float length() const
		{
			return std::sqrt(x * x + y * y + z * z);
		}

		float length2() const
		{
			return x * x + y * y + z * z;
		}

		float distance(const Vec3& rhv) const
		{
			return std::sqrt((x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y) - (z * rhv.z) * (z * rhv.z));
		}

		float distance2(const Vec3& rhv) const
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
			return x * rhv.x + y * rhv.y + z * rhv.z;
		}

		Vec3 reflect(const Vec3& rhv) const;
	};

	inline const Vec3 operator+(Vec3 lhv, const Vec3& rhv) 
	{
		lhv += rhv;
		return lhv;
	}

	inline const Vec3 operator-(Vec3 lhv, const Vec3& rhv)
	{
		lhv -= rhv;
		return lhv;
	}
	
	inline const Vec3 operator*(Vec3 lhv, const Vec3& rhv)
	{
		lhv *= rhv;
		return lhv;
	}
	
	inline const Vec3 operator/(Vec3 lhv, const Vec3& rhv)
	{
		lhv /= rhv;
		return lhv;
	}

	inline const Vec3 operator+(const Vec3& lhv, float rhv)
	{
		return Vec3{ lhv.x + rhv, lhv.y + rhv, lhv.z + rhv };
	}
	
	inline const Vec3 operator-(const Vec3& lhv, float rhv)
	{
		return Vec3{ lhv.x - rhv, lhv.y - rhv, lhv.z - rhv };
	}
	
	inline const Vec3 operator*(const Vec3& lhv, float rhv)
	{
		return Vec3{ lhv.x * rhv, lhv.y * rhv, lhv.z * rhv };
	}
	
	inline const Vec3 operator/(const Vec3& lhv, float rhv)
	{
		return Vec3{ lhv.x / rhv, lhv.y / rhv, lhv.z / rhv };
	}

	inline Vec3 Vec3::reflect(const Vec3& rhv) const
	{
		return 2.f * rhv.dot(*this) * (*this) - rhv;
	}

	inline std::ostream& operator<<(std::ostream& out, const Phoenix::Vec3& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << vec.z << " )";
		return out;
	}
}

