#pragma once

#include <Core/Component.hpp>
#include <Math/Vec3.hpp>
#include <Math/Matrix4.hpp>

namespace Phoenix
{
	struct CTransform : public Component
	{
		CTransform()
			: m_translation(0.f, 0.f, 0.f)
			, m_rotation(0.f, 0.f, 0.f)
			, m_scale(1.f, 1.f, 1.f)
		{}

		Vec3 m_translation;
		Vec3 m_scale;
		Vec3 m_rotation;

		Matrix4 toMat4() const
		{
			return Matrix4::translation(m_translation)
				* Matrix4::rotation(m_rotation)
				* Matrix4::scale(m_scale);
		}
	};
}