#pragma once

#include <Core/Component.hpp>
#include <Core/Mesh.hpp>

namespace Phoenix
{
	struct CStaticMesh : public Component
	{
		CStaticMesh() = default;

		CStaticMesh(const StaticMesh& mesh)
			: m_mesh(mesh)
		{}

		StaticMesh m_mesh;
	};
}