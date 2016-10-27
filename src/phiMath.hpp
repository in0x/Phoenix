#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec4.hpp"
#include "Matrix3.hpp"
#include "Matrix4.hpp"

namespace Phoenix::Math
{
	enum class ProjectionType
	{
		PERSPECTIVE,
		ORTHO
	};

	Matrix4 lookAtRH(vec3& cameraPos, vec3& target, vec3& up);
	Matrix4 projectionRH(float yFOV, float aspect, float near, float far, ProjectionType type);

	constexpr float pi();
	float degrees(float angleRad);
	float radians(float angleDeg);
}