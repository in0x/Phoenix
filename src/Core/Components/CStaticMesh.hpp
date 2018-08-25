#pragma once

#include <Core/Component.hpp>
#include <Core/Mesh.hpp>

namespace Phoenix
{
	class CStaticMesh : public Component
	{
	public:
		CStaticMesh() : m_mesh(nullptr) {}

		StaticMesh* m_mesh;

		virtual void save(Archive* ar) override;
		virtual void load(Archive* ar, LoadResources* resources) override;

		IMPL_EC_TYPE_ID(ECType::CT_StaticMesh, "StaticMesh");
	};

}