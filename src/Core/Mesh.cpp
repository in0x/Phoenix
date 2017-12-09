#include "Mesh.hpp"

#include <Core/obj.hpp>
#include <core/Logger.hpp>
#include <Render/RIDevice.hpp>

#include <assert.h>

namespace Phoenix
{
	RenderMesh loadRenderMesh(const char* path, IRIDevice* renderDevice)
	{
		RenderMesh mesh;

		const char* fileDot = strrchr(path, '.');
		size_t pathLen = strlen(path);
		if (nullptr == fileDot)
		{
			assert(false);
			Logger::errorf("Could not locate file ending in path %s while loading RenderMesh", path);
			return mesh;
		}

		if (strcmp(".obj", fileDot) == 0)
		{
			const char* lastSlash = strrchr(path, '/');
			lastSlash++;

			std::string pathToAsset(path, lastSlash);
			std::string assetName(lastSlash, path + pathLen);

			OBJImport import = loadObj(pathToAsset, assetName);

			mesh.m_numVertices = import.mesh.vertices.size();
			mesh.m_numIndices = import.mesh.indices.size();

			VertexBufferFormat layout;
			layout.add({ EAttributeProperty::Position, EAttributeType::Float, 3 },
			{ sizeof(Vec3), import.mesh.vertices.size(), import.mesh.vertices.data() });

			layout.add({ EAttributeProperty::Normal, EAttributeType::Float, 3 },
			{ sizeof(Vec3), import.mesh.normals.size(), import.mesh.normals.data() });

			mesh.m_vertexbuffer = renderDevice->createVertexBuffer(layout);
			mesh.m_indexbuffer = renderDevice->createIndexBuffer(sizeof(uint32_t), mesh.m_numIndices, import.mesh.indices.data());

			assert(mesh.m_vertexbuffer.isValid());
			assert(mesh.m_indexbuffer.isValid());
		}
		else
		{
			assert(false);
			Logger::errorf("Attempted to load unsuppored Mesh Type %s while loading RenderMesh", path);
		}

		return mesh;
	}
}