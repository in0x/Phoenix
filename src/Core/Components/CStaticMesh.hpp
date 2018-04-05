#pragma once

#include <Core/Component.hpp>
#include <Core/Mesh.hpp>
#include <Core/Material.hpp>

namespace Phoenix
{
	struct CStaticMesh : public Component
	{
		CStaticMesh() = default;

		CStaticMesh(const RenderMesh& mesh, const Material& material)
			: m_mesh(mesh)
			, m_material(material)
		{}

		RenderMesh m_mesh;
		Material m_material;
	};
}