#pragma once

#include "Vec3.hpp"

namespace Phoenix
{
	// Represents a point in 3D space. This allows other geometry classes
	// to differntiate between when we are working with positions as opposed  
	// to directions. Use Vec3 to represnt a directional vector.
	class Point3D
	{
	public:
		Vec3 position;

		Point3D();
		Point3D(float x, float y, float z);

		void operator+=(const Vec3& rhv);
		void operator-=(const Vec3& rhv);
	};

	inline Point3D operator+(Point3D lhv, const Vec3& rhv)
	{
		lhv += rhv;
		return lhv;
	}

	inline Point3D operator-(Point3D lhv, const Vec3& rhv)
	{
		lhv -= rhv;
		return lhv;
	}
}