#pragma once

#include <Math/Matrix4.hpp>
#include <Math/Vec3.hpp>

namespace Phoenix
{
	class Transform
	{
	public:
		Transform();

		void translate(const Vec3& translation);
		void scale(const Vec3& scale);
		void rotate(const Vec3& angles);
		
		Matrix4 m_local;
		Matrix4 m_world;
	};
}