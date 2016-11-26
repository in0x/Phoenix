#pragma once

#include "PhiCoreRequired.hpp"
#include "Vec3.hpp"

namespace Phoenix
{
	// Represents a point32 in 3D space. This allows other geometry classes
	// to differntiate between when we are working with positions as opposed  
	// to directions. Use Vec3 to represnt a directional vector.
	class Point323D
	{
	public:
		Vec3 position;

		Point323D();
		Point323D(f32 x, f32 y, f32 z);

		void operator+=(const Vec3& rhv);
		void operator-=(const Vec3& rhv);
	};

	inline Point323D operator+(Point323D lhv, const Vec3& rhv)
	{
		lhv += rhv;
		return lhv;
	}

	inline Point323D operator-(Point323D lhv, const Vec3& rhv)
	{
		lhv -= rhv;
		return lhv;
	}
}