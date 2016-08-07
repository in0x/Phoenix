#pragma once
#include "phiMath.h"

namespace Phoenix::Math
{
	// Using right-handed coordinates

	Matrix4 lookAt(Vec3& cameraPos, Vec3& target, Vec3& up)
	{
		Vec3 zAxis = (cameraPos - target).normalize();
		Vec3 xAxis = up.cross(zAxis).normalize();
		Vec3 yAxis = zAxis.cross(xAxis);

		return Matrix4 {
			xAxis.x, yAxis.x, zAxis.x, 0,
			xAxis.y, yAxis.y, zAxis.y, 0,
			xAxis.z, yAxis.z, zAxis.z, 0,
			-(xAxis.dot(cameraPos)), -(yAxis.dot(cameraPos)), -(zAxis.dot(cameraPos)), 1
		};
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