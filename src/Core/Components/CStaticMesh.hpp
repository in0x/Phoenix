#pragma once

#include <Core/Component.hpp>
#include <Core/Mesh.hpp>
#include <Core/Material.hpp>

namespace Phoenix
{
	struct CStaticMesh : public Component
	{
		CStaticMesh() = default;

		CStaticMesh(const StaticMesh& mesh, const Material& material)
			: m_mesh(mesh)
			, m_material(material)
		{}

		StaticMesh m_mesh;
		Material m_material;
	};
}