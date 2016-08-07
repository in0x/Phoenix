#include "phiMath.h"

namespace Phoenix::Math
{
	Matrix4 view(const Vec3& cameraPos, const Vec3& lookAt, const Vec3& up)
	{
		return Matrix4{};
	}

	Matrix4 perspective(float horizontalFOV, float aspectRatio, float near, float far)
	{
		return Matrix4{};
	}

	Matrix4 ortho()
	{
		return Matrix4{};
	}
}