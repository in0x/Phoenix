#pragma once

#include <Core/Component.hpp>
#include <Core/Mesh.hpp>

namespace Phoenix
{
	struct CStaticMesh : public Component
	{
		CStaticMesh() = default;

		CStaticMesh(const StaticMesh& mesh) // TODO replace with pointer
			: m_mesh(mesh)
		{}
		
		CStaticMesh(StaticMesh&& mesh)
			: m_mesh(std::move(mesh))
		{}

		StaticMesh m_mesh;
	};
}