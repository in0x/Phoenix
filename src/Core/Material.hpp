#pragma once

#include <Math/Vec3.hpp>

namespace Phoenix
{
	class Material
	{
	public:
		Material()
			: m_diffuse(Vec3())
			, m_specular(Vec3())
			, m_specularExp(0.f)
		{}

		Material(const Vec3& diffuse, const Vec3& specular, float specularExp)
			: m_diffuse(diffuse)
			, m_specular(specular)
			, m_specularExp(specularExp)
		{}

		Vec3 m_diffuse;
		Vec3 m_specular;
		float m_specularExp;
	};
}