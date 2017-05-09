#pragma once
#include <ostream>

namespace Phoenix
{
	class Vec2;
	class Vec3;
	class Vec4;
	class Matrix3;
	class Matrix4;
	class Quaternion;

	std::ostream& operator<<(std::ostream& out, const Vec2& vec);
	std::ostream& operator<<(std::ostream& out, const Vec4& vec);
	std::ostream& operator<<(std::ostream& out, const Vec3& vec);
	std::ostream& operator<<(std::ostream& out, const Matrix3& mat3);
	std::ostream& operator<<(std::ostream& out, const Matrix4& mat4);
	std::ostream& operator<<(std::ostream& out, const Quaternion& quat);
}
