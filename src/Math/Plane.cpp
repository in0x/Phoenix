#include "Plane.hpp"
#include "Common.hpp"

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

	std::pair<bool, Vec3> Plane::intersect(const Ray& ray) const
	{
		return{ false, {0,0,0} };
	}

	bool Plane::intersect(const Plane& other) const
	{
		return false;
	}

	float Plane::distance(const Vec3& point) const
	{
		return n.dot(point) - d;
	}

	Plane::Side Plane::getSideOn(const Vec3& point) const
	{
		auto dist = distance(point);
		
		if (dist < 0.f)
		{
			return Side::BACK;
		}
		else if (dist > 0.f)
		{
			return Side::FRONT;
		}

		return Side::ON;
	}

	void Plane::normalize()
	{
		auto nLen = n.length();

		if (nLen > FLT_CMP_TOLERANCE)
		{
			auto invNLen = 1.f / nLen;
			n *= nLen;
			d *= nLen;
		}
	}
}