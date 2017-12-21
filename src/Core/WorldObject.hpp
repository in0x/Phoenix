#pragma once

#include <Math/Matrix4.hpp>
#include <Math/Vec3.hpp>

#include <Core/Mesh.hpp>

namespace Phoenix
{
	class Transform
	{
	public:
		Transform() 
			: m_translation(Vec3(0.f, 0.f, 0.f))
			, m_rotation(Vec3(0.f, 0.f, 0.f))
			, m_scale(Vec3(1.f, 1.f, 1.f))
		{}

		Vec3 m_translation;
		Vec3 m_rotation;
		Vec3 m_scale;

		Matrix4 toMat4() const;
	};

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

	class WorldObject
	{
	public:
		Transform m_transform;
		Material m_material;
		RenderMesh m_mesh;
	};
}