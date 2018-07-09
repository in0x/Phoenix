#pragma once

namespace Phoenix
{
	struct StaticMesh;

	struct CStaticMesh
	{
		CStaticMesh(const StaticMesh* mesh) 
			: m_mesh(mesh)
		{}

		const StaticMesh* m_mesh;
	};
}