#pragma once

#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec4.hpp"
#include "Matrix3.hpp"
#include "Matrix4.hpp"
#include "Quaternion.hpp"
#include "Rotation.hpp"

namespace Phoenix::Math
{
	enum class ProjectionType
	{
		PERSPECTIVE,
		ORTHO
	};

	Matrix4 lookAtRH(Vec3& cameraPos, Vec3& target, Vec3& up);
	Matrix4 projectionRH(float yFOV, float aspect, float near, float far, ProjectionType type);
	
	constexpr float pi()
	{
		return 3.1415926535897932384626433832795f;
	}

	constexpr float degrees(float angleRad)
	{
		return angleRad * (180.f / pi());
	}

	constexpr float radians(float angleDeg)
	{
		return angleDeg * (pi() / 180.f);
	}

	float fastInvSqrt(float x);
}