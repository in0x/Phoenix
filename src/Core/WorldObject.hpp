#pragma once

#include <Math/Matrix4.hpp>
#include <Math/Vec3.hpp>

#include <Core/obj.hpp>
#include <Core/Mesh.hpp>

namespace Phoenix
{
	class Transform
	{
	public:
		Transform() {}

		Vec3 m_translation;
		Vec3 m_rotation;
		Vec3 m_scale;

		Matrix4 toMat4() const
		{
			return{};
		}
	};

	class Material
	{
		Vec3 m_diffuse;
		Vec3 m_specular;
		Vec3 m_shininess;
	};

	class Entity
	{
		Transform m_transform;
		Material m_material;
		Mesh* m_pMesh;
	};
}