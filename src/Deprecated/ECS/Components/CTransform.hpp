#pragma once

#include <Math/Vec3.hpp>
#include <Math/Matrix4.hpp>

namespace Phoenix
{
	struct CTransform 
	{
		CTransform()
			: m_translation(0.f, 0.f, 0.f)
			, m_rotation(0.f, 0.f, 0.f)
			, m_scale(1.f, 1.f, 1.f)
		{
			recalculate();
		}

		Vec3 m_translation;
		Vec3 m_scale;
		Vec3 m_rotation;
		Matrix4 m_cached;

		const Matrix4& recalculate() 
		{
			m_cached = Matrix4::translation(m_translation)
					 * Matrix4::rotation(m_rotation)
					 * Matrix4::scale(m_scale);
		
			return m_cached;
		}
	};
}