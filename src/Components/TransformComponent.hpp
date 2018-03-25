#pragma once

#include <Core/Component.hpp>
#include <Math/Vec3.hpp>
#include <Math/Matrix4.hpp>

namespace Phoenix
{
	struct TransformComponent : public Component<TransformComponent>
	{
		Vec3 m_translation;
		Vec3 m_scale;
		Vec3 m_rotation;

		Matrix4 toMat4() const
		{
			return Matrix4::translation(m_translation.x, m_translation.y, m_translation.z)
				* Matrix4::rotation(m_rotation.x, m_rotation.y, m_rotation.z)
				* Matrix4::scale(m_scale.x, m_scale.y, m_scale.z);
		}
	};
}
