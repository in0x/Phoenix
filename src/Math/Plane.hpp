#pragma once

#include "Vec3.hpp"

namespace Phoenix
{
	class Point3D;
	class Ray;

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

		void normalize();
		float dot(const Vec3& dir) const;
		float dot(const Point3D& point) const;

		Side getSideOn(const Vec3& point) const;
		float distance(const Vec3& point) const;
		Vec3 reflect(const Vec3& point) const;

		std::pair<bool, float> intersect(const Ray& ray) const;

		bool intersect(const Plane& other) const;
	};
}