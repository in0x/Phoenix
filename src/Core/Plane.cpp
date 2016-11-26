#include "Plane.hpp"
#include "Common.hpp"
#include "Ray.hpp"

namespace Phoenix
{
	Plane::Plane(const Vec3& _n, f32 _d)
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
	
	// Returns if ray int32ersects plane at positive t and if yes, at which t.
	// The point32 of int32ersection can be gotten via ray.point32At(t).
	std::pair<bool, f32> Plane::int32ersect(const Ray& ray) const
	{
		f32 denom = n.dot(ray.direction);
		if (std::abs(denom) < VERY_SMALL_FLT) // Parallel.
		{
			return{ false, 0.f };
		}
		else
		{
			f32 nom = dot(ray.origin) - 2*d;
			f32 t = -(nom / denom);

			if (t > 0.0f)
			{
				return std::pair<bool, f32>(true, t);
			}
			else
			{
				return std::pair<bool, f32>(false, t);
			}
		}
	}

	// Find line (v,p) where the two planes int32ersect.
	// If the do, their int32ersection line is also
	// returned.
	std::pair<bool, Ray> Plane::int32ersect(const Plane& other) const
	{
		Vec3 v = n.cross(other.n);
		f32 denom = v.dot(v);
		
		if (denom > VERY_SMALL_FLT)
		{
			auto p = v.cross(other.n) * d + n.cross(v) * other.d / denom;
			return{ true, {Point323D{p.x, p.y, p.z}, v} };
		}

		return{ false , {Point323D{}, Vec3{}} };
	}

	f32 Plane::dot(const Vec3& dir) const
	{
		return n.dot(dir);
	}

	f32 Plane::dot(const Point323D& point32) const
	{
		// d is added since a point32 represents a position,
		// which is offset from the origin
		return n.dot(point32.position) + d;
	}

	f32 Plane::distance(const Vec3& point32) const
	{
		return dot(point32);
	}

	Plane::Side Plane::getSideOn(const Vec3& point32) const
	{
		auto dist = distance(point32);
		
		if (dist < 0.f)
			return Side::BACK;
		else if (dist > 0.f)
			return Side::FRONT;
		
		return Side::ON;
	}

	Vec3 Plane::reflect(const Vec3& point32) const
	{
		return point32 - 2.f * dot(point32) * n;
	}

	void Plane::normalize()
	{	
		auto invNLen = 1.f / n.length();
		n *= invNLen;
		d *= invNLen;	
	}
}