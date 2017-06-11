#include "ObjIndexer.hpp"
#include <cassert>

namespace Phoenix
{
	std::unique_ptr<Mesh> ObjIndexer::convertForOpenGL(ObjData* loaded)
	{
		pConvertedMesh = std::make_unique<Mesh>();
		pConvertedMesh->materials.swap(loaded->materials);
		pConvertedMesh->bSmoothShading = loaded->bSmoothShading;

		bool bHasVertices = loaded->vertices.size() > 0;
		assert(bHasVertices);
		bool bHasNormals = loaded->normals.size() > 0;
		bool bHasUVs = loaded->uvs.size() > 0;

		if (bHasVertices & bHasNormals & bHasUVs)
		{
			toPacked = &ObjIndexer::toPackedVTN;
			addData = &ObjIndexer::addVTN;
		}
		else if (bHasVertices & bHasNormals & !bHasUVs)
		{
			toPacked = &ObjIndexer::toPackedVN;
			addData = &ObjIndexer::addVN;
		}
		else if (bHasVertices & bHasUVs & !bHasNormals)
		{
			toPacked = &ObjIndexer::toPackedVT;
			addData = &ObjIndexer::addVT;
		}
		else // bHasVertices & !bHasNormals & !bHasUVs 
		{
			toPacked = &ObjIndexer::toPackedV;
			addData = &ObjIndexer::addV;
		}

		size_t vertexCount = pConvertedMesh->vertices.size();
		pConvertedMesh->vertices.reserve(vertexCount);

		if (loaded->normals.size() > 0)
		{
			pConvertedMesh->normals.reserve(vertexCount);
		}

		if (loaded->normals.size() > 0)
		{
			pConvertedMesh->uvs.reserve(vertexCount);
		}

		for (const Face& face : loaded->faces)
		{
			PackedVertexData packedVertices[3];
			(this->*toPacked)(packedVertices, face, loaded);

			for (const auto& data : packedVertices)
			{
				unsigned int index;
				bool bDataExists = doesDataExist(data, index);

				if (bDataExists)
				{
					pConvertedMesh->indices.push_back(index);
				}
				else
				{
					addDataAndIndex(data);
				}
			}
		}

		return std::move(pConvertedMesh);
	}

	bool ObjIndexer::doesDataExist(const PackedVertexData& data, unsigned int& outIndex)
	{
		auto iter = packed.find(data);

		if (iter == packed.end())
		{
			return false;
		}
		else {
			outIndex = iter->second;
			return true;
		}
	}

	void ObjIndexer::toPackedV(PackedVertexData* dataArr, const Face& face, const ObjData* loaded)
	{
		dataArr[0].vertex = loaded->vertices[face.vertexIndices[0]];
		dataArr[1].vertex = loaded->vertices[face.vertexIndices[1]];
		dataArr[2].vertex = loaded->vertices[face.vertexIndices[2]];
	}

	void ObjIndexer::toPackedVN(PackedVertexData* dataArr, const Face& face, const ObjData* loaded)
	{
		dataArr[0].vertex = loaded->vertices[face.vertexIndices[0]];
		dataArr[0].normal = loaded->normals[face.normalIndices[0]];

		dataArr[1].vertex = loaded->vertices[face.vertexIndices[1]];
		dataArr[1].normal = loaded->normals[face.normalIndices[1]];

		dataArr[2].vertex = loaded->vertices[face.vertexIndices[2]];
		dataArr[2].normal = loaded->normals[face.normalIndices[2]];
	}

	void ObjIndexer::toPackedVT(PackedVertexData* dataArr, const Face& face, const ObjData* loaded)
	{
		dataArr[0].vertex = loaded->vertices[face.vertexIndices[0]];
		dataArr[0].uv = loaded->uvs[face.uvIndices[0]];

		dataArr[1].vertex = loaded->vertices[face.vertexIndices[1]];
		dataArr[1].uv = loaded->uvs[face.uvIndices[1]];

		dataArr[2].vertex = loaded->vertices[face.vertexIndices[2]];
		dataArr[2].uv = loaded->uvs[face.uvIndices[2]];
	}

	void ObjIndexer::toPackedVTN(PackedVertexData* dataArr, const Face& face, const ObjData* loaded)
	{
		dataArr[0].vertex = loaded->vertices[face.vertexIndices[0]];
		dataArr[0].normal = loaded->normals[face.normalIndices[0]];
		dataArr[0].uv = loaded->uvs[face.uvIndices[0]];

		dataArr[1].vertex = loaded->vertices[face.vertexIndices[1]];
		dataArr[1].normal = loaded->normals[face.normalIndices[1]];
		dataArr[1].uv = loaded->uvs[face.uvIndices[1]];

		dataArr[2].vertex = loaded->vertices[face.vertexIndices[2]];
		dataArr[2].normal = loaded->normals[face.normalIndices[2]];
		dataArr[2].uv = loaded->uvs[face.uvIndices[2]];
	}

	void ObjIndexer::addV(const PackedVertexData& data)
	{
		pConvertedMesh->vertices.push_back(data.vertex);
	}

	void ObjIndexer::addVN(const PackedVertexData& data)
	{
		pConvertedMesh->vertices.push_back(data.vertex);
		pConvertedMesh->normals.push_back(data.normal);
	}

	void ObjIndexer::addVT(const PackedVertexData& data)
	{
		pConvertedMesh->vertices.push_back(data.vertex);
		pConvertedMesh->uvs.push_back(data.uv);
	}

	void ObjIndexer::addVTN(const PackedVertexData& data)
	{
		pConvertedMesh->vertices.push_back(data.vertex);
		pConvertedMesh->normals.push_back(data.normal);
		pConvertedMesh->uvs.push_back(data.uv);
	}

	void ObjIndexer::addDataAndIndex(const PackedVertexData& data)
	{
		(this->*addData)(data);

		unsigned int index = static_cast<unsigned int>(packed.size());
		packed[data] = index;
		pConvertedMesh->indices.push_back(index);
	}
}
