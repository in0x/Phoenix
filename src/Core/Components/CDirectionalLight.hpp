#pragma once

#include <Core/Component.hpp>
#include <Math/Vec3.hpp>

namespace Phoenix
{
	struct CDirectionalLight : public Component
	{
		CDirectionalLight(const Vec3& direction, const Vec3& color)
			: m_direction(direction)
			, m_color(color)
		{}

		Vec3 m_direction;
		Vec3 m_color;
	};
}