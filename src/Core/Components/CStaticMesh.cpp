#include "CStaticMesh.hpp"

#include <Core/Serialize.hpp>
#include <Core/SerialUtil.hpp>

namespace Phoenix
{
	void CStaticMesh::save(Archive* ar) 
	{
		serialize(ar, m_mesh->m_name);
	}

	void CStaticMesh::load(Archive* ar, LoadResources* resources) 
	{
		std::string meshName;
		serialize(ar, meshName);
		m_mesh = loadStaticMesh(meshName.c_str(), resources);
	}
}