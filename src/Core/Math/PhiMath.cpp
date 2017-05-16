#include "PhiMath.hpp"

namespace Phoenix
{
	// Uses the fast inverse sqrt trick also used in Quake3.
	float fastInvSqrt(float x)
	{
		float xhalf = 0.5f * x;
		int i = *(int*)&x;      // store floating-point bits in integer
		i = 0x5f3759df - (i >> 1);  // initial guess for Newton's method
		x = *(float*)&i;              // convert new bits into float
		x = x*(1.5f - xhalf*x*x);   // One round of Newton's method
		return x;
	}

	Matrix4 lookAtRH(Vec3& cameraPos, Vec3& target, Vec3& up)
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
		return Matrix4{
			xAxis.x, xAxis.y, xAxis.z, -(xAxis.dot(cameraPos)),
			yAxis.x, yAxis.y, yAxis.z, -(yAxis.dot(cameraPos)),
			zAxis.x, zAxis.y, zAxis.z, -(zAxis.dot(cameraPos)),
			0,		 0,		  0,		1
		};
	}

	Matrix4 perspectiveRH(float yFOV, float aspect, float near, float far)
	{
		float tanTheta2 = std::tan(yFOV * (PI / 360.f));

		return Matrix4{
			1.f / tanTheta2 ,0, 0, 0,
			0, aspect / tanTheta2, 0, 0,
			0, 0, (near + far) / (near - far), ((2.f * near * far) / (near - far)),
			0, 0, -1, 0
		};
	}

	Matrix4 orthographicRH(float yFOV, float aspect, float near, float far)
	{
		float scale = std::tan(yFOV * 0.5f * PI / 180.f) * near;
		float right = aspect * scale;
		float left = -right;
		float top = scale;
		float bottom = -top;

		float a = 2 * near / (right - left);
		float b = 2 * near / (top - bottom);
		float c = (right + left) / (right - left);
		float d = (top + bottom) / (top - bottom);
		float e = -(far + near) / (far - near);
		float f = (-2 * far * near) / (far - near);

		return Matrix4 {
			a, 0, c, 0,
			0, b, d, 0,
			0, 0, e, f,
			0, 0, -1, 0
		};	
	}
}

