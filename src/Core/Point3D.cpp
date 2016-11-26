#include "Point3D.hpp"

namespace Phoenix
{
	Point3D::Point3D()
	{
	}

	Point3D::Point3D(float x, float y, float z)
		: position(x,y,z)
	{
	}

	void Point3D::operator+=(const Vec3& rhv)
	{
		position += rhv;
	}

	void Point3D::operator-=(const Vec3& rhv)
	{
		position -= rhv;
	}
}