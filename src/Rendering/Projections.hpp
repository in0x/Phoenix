#pragma once
#include "../Math/PhiMath.hpp"

namespace Phoenix::Render
{
	enum class ProjectionType
	{
	PERSPECTIVE,
	ORTHO
	};

	Matrix4 lookAtRH(Vec3& cameraPos, Vec3& target, Vec3& up);
	Matrix4 projectionRH(float yFOV, float aspect, float near, float far, ProjectionType type);	
}
