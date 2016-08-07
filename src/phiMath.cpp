#pragma once
#include "phiMath.h"

namespace Phoenix::Math
{
	// Using right-handed coordinates

	Matrix4 lookAt(Vec3& cameraPos, Vec3& target, Vec3& up)
	{
		// Construct basis vectors
		Vec3 zAxis = (cameraPos - target).normalize();
		Vec3 xAxis = up.cross(zAxis).normalize();
		Vec3 yAxis = zAxis.cross(xAxis);

		// Concatenate new orientation and translation
		// Taking the dot of basis vector and translation component
		// has the same result as constructing a translation matrix
		// using -cameraPos and multiplying it with the orientation 
		// single column matrix multipl. == dot
		return Matrix4 {
			xAxis.x, xAxis.y, xAxis.z, -(xAxis.dot(cameraPos)),
			yAxis.x, yAxis.y, yAxis.z, -(yAxis.dot(cameraPos)),
			zAxis.x, zAxis.y, zAxis.z, -(zAxis.dot(cameraPos)),
			0,		 0,		  0,		1
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