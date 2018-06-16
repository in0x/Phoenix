#pragma once

#include <ECS/ComponentTypes.hpp>

namespace Phoenix
{
	struct StaticMesh;

	struct CStaticMesh
	{
		CStaticMesh(const StaticMesh* mesh) 
			: m_mesh(mesh)
		{}

		static const uint64_t s_type = EComponent::StaticMesh;

		const StaticMesh* m_mesh;
	};
}