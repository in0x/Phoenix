#pragma once


#include <Core/Component.hpp>
#include <Math/Vec3.hpp>

namespace Phoenix
{
	struct MaterialComponent : Component<MaterialComponent>
	{
		MaterialComponent()
			: m_diffuse()
			, m_specular()
			, m_specularExp(0.0f)
		{}

		Vec3 m_diffuse;
		Vec3 m_specular;
		float m_specularExp;
	};
}