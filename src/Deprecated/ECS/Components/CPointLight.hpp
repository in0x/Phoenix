#pragma once

#include <Math/Vec3.hpp>

namespace Phoenix
{
	struct CPointLight 
	{
		CPointLight(const Vec3& color, float radius, float intensity)
			: m_color(color)
			, m_radius(radius)
			, m_intensity(intensity)
		{}

		Vec3 m_color;
		float m_radius;
		float m_intensity;
	};
}