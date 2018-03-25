#pragma once

#pragma once

#include <Core/Component.hpp>
#include <Math/PhiMath.hpp>

namespace Phoenix
{
	struct CameraComponent : Component<CameraComponent>
	{
		CameraComponent(float horizontalFOV, float screenWidth, float screenHeight, float near, float far)
			: m_horizontalFOV(horizontalFOV)
			, m_screenWidth(screenWidth)
			, m_screenHeight(screenHeight)
			, m_near(near)
			, m_far(far)
		{}

		Matrix4 getProjection()
		{
			return perspectiveRH(m_horizontalFOV, m_screenWidth / m_screenHeight, m_near, m_far);
		}

		float m_horizontalFOV;
		float m_screenWidth;
		float m_screenHeight;
		float m_near;
		float m_far;
	};
}