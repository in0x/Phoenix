#pragma once

#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec4.hpp"
#include "Matrix3.hpp"
#include "Matrix4.hpp"
#include "Quaternion.hpp"

namespace Phoenix::Math
{
	enum class ProjectionType
	{
		PERSPECTIVE,
		ORTHO
	};

	Matrix4 lookAtRH(Vec3& cameraPos, Vec3& target, Vec3& up);
	Matrix4 projectionRH(float yFOV, float aspect, float near, float far, ProjectionType type);

	constexpr float pi();
	float degrees(float angleRad);
	float radians(float angleDeg);
	float fastInvSqrt(float x);
}