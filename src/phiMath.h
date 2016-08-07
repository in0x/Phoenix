#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec4.hpp"
#include "Matrix4.hpp"

namespace Phoenix::Math
{
	// Constructs a ViewMatrix
	Matrix4 lookAt(Vec3& cameraPos, Vec3& target, Vec3& up);

	Matrix4 perspective(float horizontalFOV, float aspectRatio, float near, float far);

	Matrix4 ortho();
}