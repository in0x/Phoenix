#pragma once

#include <Core/Component.hpp>
#include <Core/Mesh.hpp>

namespace Phoenix
{
	struct CStaticMesh : public Component
	{
		CStaticMesh(const StaticMesh* mesh) 
			: m_mesh(mesh)
		{}
		
		const StaticMesh* m_mesh;
	};
}