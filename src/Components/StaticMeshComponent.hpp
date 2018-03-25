#pragma once

#include <Core/Component.hpp>
#include <Core/Mesh.hpp>
#include <Core/Material.hpp>
	
namespace Phoenix
{
	struct StaticMeshComponent : public Component<StaticMeshComponent>
	{
		RenderMesh m_mesh;
	};
}
