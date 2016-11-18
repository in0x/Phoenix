#pragma once

#include "Vec3.hpp"

namespace Phoenix
{
	class Ray
	{
	private:
		Vec3 m_origin;
		Vec3 m_direction;

	public:
		Ray(const Vec3& position, const Vec3& direction);

		Vec3 pointAt(float t) const;
	};
}