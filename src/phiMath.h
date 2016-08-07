#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec4.hpp"
#include "Matrix4.hpp"

namespace Phoenix::Math
{
	static Matrix4 view(const Vec3& cameraPos, const Vec3& lookAt, const Vec3& up);
	static Matrix4 perspective(float horizontalFOV, float aspectRatio, float near, float far);
	static Matrix4 ortho();
}