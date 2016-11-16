#include "Common.hpp"
#include <cmath>

namespace Phoenix::Math
{

	/*
	Header stuff
	enum class ProjectionType
	{
	PERSPECTIVE,
	ORTHO
	};

	Matrix4 lookAtRH(Vec3& cameraPos, Vec3& target, Vec3& up);
	Matrix4 projectionRH(float yFOV, float aspect, float near, float far, ProjectionType type);

	*/

	// Currently only supporting OpenGL

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
		return Matrix4 {
			xAxis.x, xAxis.y, xAxis.z, -(xAxis.dot(cameraPos)),
			yAxis.x, yAxis.y, yAxis.z, -(yAxis.dot(cameraPos)),
			zAxis.x, zAxis.y, zAxis.z, -(zAxis.dot(cameraPos)),
			0,		 0,		  0,		1
		};
	}

	Matrix4 projectionRH(float yFOV, float aspect, float near, float far, ProjectionType type)
	{
		float xFOV = yFOV / aspect;
		float zoomX = 1.f / std::tan(xFOV / 2.f);
		float zoomY = 1.f / std::tan(yFOV / 2.f);

		if (type == ProjectionType::PERSPECTIVE)
		{
			return Matrix4 {
				zoomX, 0, 0, 0,
				0, zoomY, 0, 0,
				0, 0, -((far + near) / (far - near)), ((-2.f * near * far) / (far - near)),
				0, 0, -1, 0
			};
		}
		else
		{
			return Matrix4 {
				zoomX, 0, 0, 0,
				0, zoomY, 0, 0,
				0, 0, -(2.f / (far - near)), -((far + near) / (far - near)),
				0, 0, 0, 1
			};
		}
	}

	// Uses the fast inverse sqrt trick also used in Quake3.
	float fastInvSqrt(float x)
	{
        float xhalf = 0.5f * x;
        int i = *(int*)&x;            // store floating-point bits in integer
        i = 0x5f3759df - (i >> 1);    // initial guess for Newton's method
        x = *(float*)&i;              // convert new bits into float
        x = x*(1.5f - xhalf*x*x);     // One round of Newton's method
        return x;
	}
}