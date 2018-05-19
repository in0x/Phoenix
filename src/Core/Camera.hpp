#pragma once

#include <Math/Vec3.hpp>
#include <Math/Matrix4.hpp>

namespace Phoenix
{
	struct Camera
	{
		Vec3 m_position;
		Vec3 m_xAxis;
		Vec3 m_zAxis;
		Vec3 m_yAxis;

		Camera();

		void moveRight(float delta);
		void moveForward(float delta);

		void pitch(float angle);
		void yaw(float angle);

		Matrix4 getUpdatedViewMatrix();
	};
}