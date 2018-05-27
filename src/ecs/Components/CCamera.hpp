#pragma once

#include <Core/Component.hpp>
#include <Math/Matrix4.hpp>
#include <Math/PhiMath.hpp>

namespace Phoenix
{
	struct CCamera : public Component
	{
		CCamera(float horizontalFOV, float screenWidth, float screenHeight, float near, float far)
			: m_horizontalFOV(horizontalFOV)
			, m_screenWidth(screenWidth)
			, m_screenHeight(screenHeight)
			, m_near(near)
			, m_far(far)
		{}

		float m_horizontalFOV;
		float m_screenWidth;
		float m_screenHeight;
		float m_near;
		float m_far;
	};
}