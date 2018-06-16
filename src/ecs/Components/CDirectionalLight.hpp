#pragma once

#include <ECS/ComponentTypes.hpp>
#include <Math/Vec3.hpp>

namespace Phoenix
{
	struct CDirectionalLight 
	{
		CDirectionalLight(const Vec3& direction, const Vec3& color)
			: m_direction(direction)
			, m_color(color)
		{}

		static const uint64_t s_type = EComponent::DirLight;

		Vec3 m_direction;
		Vec3 m_color;
	};
}