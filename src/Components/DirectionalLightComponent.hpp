#pragma once

#include <Core/Component.hpp>
#include <Math/Vec3.hpp>

namespace Phoenix
{
	struct DirectionLightComponent : public Component<DirectionLightComponent>
	{
		Vec3 m_direction;
		Vec3 m_color;
	};
}
