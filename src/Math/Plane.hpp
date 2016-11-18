#pragma once

#include "Vec3.hpp"
#include "Ray.hpp"

namespace Phoenix::Math
{
	// Implicit representation of plane using normal vector
	// and dot between normal and point on plane. Other point
	// can be discarded since d = dot(n, p) is constant for all 
	// points on the plane.
	class Plane
	{
	public:
		
		enum class Side
		{
			FRONT, // Halfspace toward which normal points
			BACK,
			ON
		};

		Vec3 n;
		float d;

		Plane(const Vec3& _n, float _d);
		Plane(const Plane& other);
		Plane(const Vec3& p0, const Vec3& p1, const Vec3& p2);

		// Returns if ray intersects plane and if yes, where.
		std::pair<bool, Vec3> intersect(const Ray& ray) const;

		bool intersect(const Plane& other) const;

		Side getSide(const Vec3& point) const;

		float distance(const Vec3& point) const;

		void normalize();
	};
}