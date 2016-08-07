#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec4.hpp"
#include "Matrix4.hpp"

namespace Phoenix::Math
{
	Matrix4 view(const Vec3& cameraPos, const Vec3& lookAt, const Vec3& up);
	Matrix4 perspective(float horizontalFOV, float aspectRatio, float near, float far);
	Matrix4 ortho();
}