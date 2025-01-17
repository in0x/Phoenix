#include "Ray.hpp"
#include "Plane.hpp"
#include "PhiMath.hpp"

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
		if (std::abs(denom) < VERY_SMALL_FLT) // Parallel.
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

	// Find line (v,p) where the two planes intersect.
	// If the do, their intersection line is also
	// returned.
	std::pair<bool, Ray> Plane::intersect(const Plane& other) const
	{
		Vec3 v = n.cross(other.n);
		float denom = v.dot(v);
		
		if (denom > VERY_SMALL_FLT)
		{
			auto p = v.cross(other.n) * d + n.cross(v) * other.d / denom;
			return{ true, {Vec3{p.x, p.y, p.z}, v} };
		}

		return{ false , {Vec3{}, Vec3{}} };
	}

	float Plane::dot(const Vec3& point) const
	{
		return n.dot(point) + d;
	}

	float Plane::distance(const Vec3& point) const
	{
		return dot(point);
	}

	Plane::ESide Plane::getSideOn(const Vec3& point) const
	{
		auto dist = distance(point);
		
		if (dist < 0.f)
			return ESide::BACK;
		else if (dist > 0.f)
			return ESide::FRONT;
		
		return ESide::ON;
	}

	Vec3 Plane::reflect(const Vec3& point) const
	{
		return point - n * 2.f * dot(point);
	}

	void Plane::normalize()
	{	
		auto invNLen = 1.f / n.length();
		n *= invNLen;
		d *= invNLen;	
	}
}