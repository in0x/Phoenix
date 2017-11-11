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

			mesh.numVertices = import.mesh.vertices.size();
			mesh.numIndices = import.mesh.indices.size();

			VertexBufferFormat layout;
			layout.add({ EAttributeProperty::Position, EAttributeType::Float, 3 },
			{ sizeof(Vec3), mesh.numVertices, import.mesh.vertices.data() });

			layout.add({ EAttributeProperty::Normal, EAttributeType::Float, 3 },
			{ sizeof(Vec3), mesh.numIndices, import.mesh.normals.data() });

			mesh.vb = renderDevice->createVertexBuffer(layout);
			mesh.ib = renderDevice->createIndexBuffer(sizeof(uint32_t), mesh.numIndices, import.mesh.indices.data());

			assert(mesh.vb.isValid());
			assert(mesh.ib.isValid());
		}
		else
		{
			assert(false);
			Logger::errorf("Attempted to load unsuppored Mesh Type %s while loading RenderMesh", path);
		}

		return mesh;
	}
}