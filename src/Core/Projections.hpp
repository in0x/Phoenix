#pragma once

#include "PhiCoreRequired.hpp"
#include "Vec3.hpp"
#include "Matrix4.hpp"

namespace Phoenix::Rendering
{
	enum class ProjectionType
	{
	PERSPECTIVE,
	ORTHO
	};

	Matrix4 lookAtRH(Vec3& cameraPos, Vec3& target, Vec3& up);
	Matrix4 projectionRH(f32 yFOV, f32 aspect, f32 near, f32 far, ProjectionType type);	
}
