#include "MathStreamOverloads.hpp"
#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec4.hpp"
#include "Matrix3.hpp"
#include "Matrix4.hpp"
#include "Quaternion.hpp"

namespace Phoenix
{
	std::ostream& operator<<(std::ostream& out, const Vec4& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << vec.z << " | " << vec.w << " )";
		return out;
	}

	std::ostream& operator<<(std::ostream& out, const Vec2& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << " )";
		return out;
	}

	std::ostream& operator<<(std::ostream& out, const Vec3& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << vec.z << " )";
		return out;
	}

	std::ostream& operator<<(std::ostream& out, const Matrix3& mat3)
	{
		out << mat3(0, 0) << " " << mat3(0, 1) << " " << mat3(0, 2) << "\n"
			<< mat3(1, 0) << " " << mat3(1, 1) << " " << mat3(1, 2) << "\n"
			<< mat3(2, 0) << " " << mat3(2, 1) << " " << mat3(2, 2);
		return out;
	}

	std::ostream& operator<<(std::ostream& out, const Matrix4& mat4)
	{
		out << mat4(0, 0) << " " << mat4(0, 1) << " " << mat4(0, 2) << " " << mat4(0, 3) << "\n"
			<< mat4(1, 0) << " " << mat4(1, 1) << " " << mat4(1, 2) << " " << mat4(1, 3) << "\n"
			<< mat4(2, 0) << " " << mat4(2, 1) << " " << mat4(2, 2) << " " << mat4(2, 3) << "\n"
			<< mat4(3, 0) << " " << mat4(3, 1) << " " << mat4(3, 2) << " " << mat4(3, 3);
		return out;
	}

	std::ostream& operator<<(std::ostream& out, const Quaternion& quat)
	{
		out << "( w: " << quat.w << " x: " << quat.x << " y: " << quat.y << " z: " << quat.z << " )";
		return out;
	}
}