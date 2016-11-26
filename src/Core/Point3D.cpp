#include "Point3D.hpp"

namespace Phoenix
{
	Point323D::Point323D()
	{
	}

	Point323D::Point323D(f32 x, f32 y, f32 z)
		: position(x,y,z)
	{
	}

	void Point323D::operator+=(const Vec3& rhv)
	{
		position += rhv;
	}

	void Point323D::operator-=(const Vec3& rhv)
	{
		position -= rhv;
	}
}