#include "WorldObject.hpp"

namespace Phoenix
{
	Matrix4 Transform::toMat4() const
	{
		return Matrix4::scale(m_scale.x, m_scale.y, m_scale.z)
			* Matrix4::rotation(m_rotation.x, m_rotation.y, m_rotation.z)
			* Matrix4::translation(m_translation.x, m_translation.y, m_translation.z);
	}
}