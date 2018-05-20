#pragma once

#include <Core/Component.hpp>
#include <Math/Vec3.hpp>

namespace Phoenix
{
	struct CPointLight : public Component
	{
		CPointLight(const Vec3& color, float radius)
			: m_color(color)
			, m_radius(radius)
		{}

		Vec3 m_color;
		float m_radius;
	};
}