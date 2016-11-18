#include "Ray.hpp"

namespace Phoenix
{
	Ray::Ray(const Vec3& position, const Vec3& direction)
		: m_origin(position)
		, m_direction(direction)
	{
		m_direction.normalize();
	}


	Vec3 Ray::pointAt(float t) const
	{
		return m_origin + (m_direction * t);
	}
}