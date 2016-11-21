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
		d = -n.dot(p0);
	}

	std::pair<bool, Point3D> Plane::intersect(const Ray& ray) const
	{
		float fv = dot(ray.direction);

		if (std::abs(fv) > std::numeric_limits<float>::epsilon())
		{
			return{ true, ray.origin - ray.direction * (dot(ray.origin) / fv) };
		}
		return{ false, Point3D{} };
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