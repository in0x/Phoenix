#include "Plane.hpp"
#include "Common.hpp"
#include "Ray.hpp"

namespace Phoenix
{
	Plane::Plane(const Vec3& _n, float _d)
		: n(_n)
		, d(_d)
	{ 
		n.normalize();
	}

	Plane::Plane(const Plane& other)
		: n(other.n)
		, d(other.d)
	{}

	Plane::Plane(const Vec3& p0, const Vec3& p1, const Vec3& p2)
	{
		auto edge1 = p1 - p0;
		auto edge2 = p2 - p0;

		n = edge1.cross(edge2);
		n.normalize();
		d = n.dot(p0);
	}
	
	// Returns if ray intersects plane at positive t and if yes, at which t.
	// The point of intersection can be gotten via ray.pointAt(t).
	std::pair<bool, float> Plane::intersect(const Ray& ray) const
	{
		float denom = n.dot(ray.direction);
		if (std::abs(denom) < std::numeric_limits<float>::epsilon()) // Parallel.
		{
			return{ false, 0.f };
		}
		else
		{
			float nom = dot(ray.origin) - 2*d;
			float t = -(nom / denom);

			if (t > 0.0f)
			{
				return std::pair<bool, float>(true, t);
			}
			else
			{
				return std::pair<bool, float>(false, t);
			}
		}
	}

	bool Plane::intersect(const Plane& other) const
	{
		return false;
	}

	float Plane::dot(const Vec3& dir) const
	{
		return n.dot(dir);
	}

	float Plane::dot(const Point3D& point) const
	{
		return n.dot(point.position) + d;
	}

	float Plane::distance(const Vec3& point) const
	{
		return dot(point);
	}

	Plane::Side Plane::getSideOn(const Vec3& point) const
	{
		auto dist = distance(point);
		
		if (dist < 0.f)
			return Side::BACK;
		else if (dist > 0.f)
			return Side::FRONT;
		
		return Side::ON;
	}

	Vec3 Plane::reflect(const Vec3& point) const
	{
		return point - 2.f * dot(point) * n;
	}

	void Plane::normalize()
	{	
		auto invNLen = 1.f / n.length();
		n *= invNLen;
		d *= invNLen;
		
	}
}