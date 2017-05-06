#pragma once

#include "Transform.h"

namespace Phoenix
{
	Transform::Transform()
		: m_local(Matrix4::identity())
		, m_world(Matrix4::identity())
	{
	}
	
	void Transform::translate(const Vec3& translation)
	{
		m_local = Matrix4::translation(translation.x, translation.y, translation.z) * m_local;
	}
	
	void Transform::scale(const Vec3& scale)
	{
		m_local = Matrix4::scale(scale.x, scale.y, scale.z) * m_local;
	}
	
	void Transform::rotate(const Vec3& angles)
	{
		m_local = Matrix3::fromEulerAngles(angles).asMatrix4() * m_local;
	}
}
