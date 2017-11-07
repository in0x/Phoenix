#include "obj.hpp"

#include <functional>
#include <algorithm>
#include <cassert>
#include <map>

#include <Core/StringTokenizer.hpp>
#include <Core/Logger.hpp>

namespace Phoenix
{
	struct Face
	{
		int vertexIndices[3];
		int normalIndices[3];
		int uvIndices[3];
	};

	struct ObjData
	{
		std::vector<Vec3> vertices;
		std::vector<Vec2> uvs;
		std::vector<Vec3> normals;
		std::vector<Face> faces;
		std::vector<MTL> materials;
		bool bSmoothShading;
	};

	class ObjIndexer
	{
		struct PackedVertexData
		{
			Vec3 vertex;
			Vec3 normal;
			Vec2 uv;

			bool operator<(const PackedVertexData rhv) const
			{
				return memcmp((void*)this, (void*)&rhv, sizeof(PackedVertexData)) > 0;
			};
		};

		std::map<PackedVertexData, unsigned int> packed;
		std::unique_ptr<Mesh> pConvertedMesh;

	public:
		std::unique_ptr<Mesh> convertForOpenGL(ObjData* loaded);

	private:
		void(ObjIndexer::*toPacked)(PackedVertexData*, const Face&, const ObjData*);
		void(ObjIndexer::*addData)(const PackedVertexData&);

		bool doesDataExist(const PackedVertexData& data, unsigned int& outIndex);

		void toPackedV(PackedVertexData* dataArr, const Face& face, const ObjData* loaded);
		void toPackedVN(PackedVertexData* dataArr, const Face& face, const ObjData* loaded);
		void toPackedVT(PackedVertexData* dataArr, const Face& face, const ObjData* loaded);
		void toPackedVTN(PackedVertexData* dataArr, const Face& face, const ObjData* loaded);

		void addV(const PackedVertexData& data);
		void addVN(const PackedVertexData& data);
		void addVT(const PackedVertexData& data);
		void addVTN(const PackedVertexData& data);

		void addDataAndIndex(const PackedVertexData& data);
	};

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

	class ObjParser
	{
	public:	
		std::unique_ptr<Mesh> parseOBJ(const std::string& pathTo, const std::string& name);

	private:
		void(ObjParser::*vertexParser)(std::vector<char*>&, int) = nullptr;
		char* faceDelimiter = nullptr;
		ObjData* pMeshRaw = nullptr;

		void parseVertex(std::vector<char*>& tokens);
		void parseNormal(std::vector<char*>& tokens);
		void parseUV(std::vector<char*>& tokens);

		int mapFaceIndex(int index, size_t elementCount);

		void parseFaceVertexV(std::vector<char*>& tokens, int idx);
		void parseFaceVertexVN(std::vector<char*>& tokens, int idx);
		void parseFaceVertexVT(std::vector<char*>& tokens, int idx);
		void parseFaceVertexVTN(std::vector<char*>& tokens, int idx);

		void parseFace(std::vector<char*>& tokens);
		void sanetizePath(std::string& path);
		FILE* openFile(std::string& path);

		void parseMTL(std::string& path);
		void selectFaceType(const char* faceToken);
	};

	// v -> Vertex(x, y, z, [w])
	void ObjParser::parseVertex(std::vector<char*>& tokens)
	{
		if (tokens.size() < 4)
		{
			Logger::error("Cannot parse vertex, not enough tokens.");
			assert(false);
		}

		pMeshRaw->vertices.push_back(Vec3{ strToFloat(tokens[1]),
										 strToFloat(tokens[2]),
										 strToFloat(tokens[3]) });
	}

	// vn -> Vertex Normal(x, y, z)
	void ObjParser::parseNormal(std::vector<char*>& tokens)
	{
		if (tokens.size() < 4)
		{
			Logger::error("Cannot parse normal, not enough tokens.");
			assert(false);
		}

		pMeshRaw->normals.push_back(Vec3{ strToFloat(tokens[1]),
										strToFloat(tokens[2]),
										strToFloat(tokens[3]) });
	}

	// vt -> Texture Coord(u, v, [w])
	void ObjParser::parseUV(std::vector<char*>& tokens)
	{
		if (tokens.size() < 3)
		{
			Logger::error("Cannot parse uv, not enough tokens.");
			assert(false);
		}

		pMeshRaw->uvs.push_back(Vec2{ strToFloat(tokens[1]),
									strToFloat(tokens[2]) });
	}

	int ObjParser::mapFaceIndex(int index, size_t elementCount)
	{
		if (index < 0)
		{
			index = static_cast<int>(elementCount) + index;
		}
		else
		{
			// Subtract 1 from indices because obj is 1-based.
			index -= 1;
		}

		return index;
	}

	void ObjParser::parseFaceVertexV(std::vector<char*>& tokens, int idx)
	{
		pMeshRaw->faces.back().vertexIndices[idx] = mapFaceIndex(strToInt(tokens[0]), pMeshRaw->vertices.size());
	}

	void ObjParser::parseFaceVertexVN(std::vector<char*>& tokens, int idx)
	{
		pMeshRaw->faces.back().vertexIndices[idx] = mapFaceIndex(strToInt(tokens[0]), pMeshRaw->vertices.size());
		pMeshRaw->faces.back().normalIndices[idx] = mapFaceIndex(strToInt(tokens[1]), pMeshRaw->normals.size());
	}

	void ObjParser::parseFaceVertexVT(std::vector<char*>& tokens, int idx)
	{
		pMeshRaw->faces.back().vertexIndices[idx] = mapFaceIndex(strToInt(tokens[0]), pMeshRaw->vertices.size());
		pMeshRaw->faces.back().uvIndices[idx] = mapFaceIndex(strToInt(tokens[1]), pMeshRaw->uvs.size());
	}

	void ObjParser::parseFaceVertexVTN(std::vector<char*>& tokens, int idx)
	{
		pMeshRaw->faces.back().vertexIndices[idx] = mapFaceIndex(strToInt(tokens[0]), pMeshRaw->vertices.size());
		pMeshRaw->faces.back().uvIndices[idx] = mapFaceIndex(strToInt(tokens[1]), pMeshRaw->uvs.size());
		pMeshRaw->faces.back().normalIndices[idx] = mapFaceIndex(strToInt(tokens[2]), pMeshRaw->normals.size());
	}

	/* f -> Face
	* Texture or normal can be missing
	*/
	void ObjParser::parseFace(std::vector<char*>& tokens)
	{
		if (tokens.size() < 4)
		{
			Logger::error("Cannot parse face, not enough tokens.");
			assert(false);
		}

		size_t tokenCount = tokens.size();
		size_t indexCount = tokenCount - 1;

		std::vector<std::vector<char*>> indices;
		indices.reserve(indexCount);

		for (size_t i = 1; i < tokenCount; ++i)
		{
			indices.emplace_back(tokenize(tokens[i], faceDelimiter));
		}

		pMeshRaw->faces.emplace_back();
		(this->*vertexParser)(indices[0], 0);
		(this->*vertexParser)(indices[1], 1);
		(this->*vertexParser)(indices[2], 2);

		for (size_t i = 3; i < indexCount; ++i)
		{
			pMeshRaw->faces.emplace_back();
			(this->*vertexParser)(indices[0], 0);
			(this->*vertexParser)(indices[i - 1], 1);
			(this->*vertexParser)(indices[i], 2);
		}
	}

	void ObjParser::sanetizePath(std::string& path)
	{
		char tokensToRemove[] = { '\n', '\r', ' ' };

		for (char toRemove : tokensToRemove)
		{
			size_t tokenPos = 0;

			while ((tokenPos = path.find(toRemove, tokenPos)) != std::string::npos)
			{
				path.erase(tokenPos, 1);
			}
		}
	}

	FILE* ObjParser::openFile(std::string& path)
	{
		sanetizePath(path);
		FILE* file = fopen(path.c_str(), "r");

		if (!file)
		{	
			Logger::errorf("Failed to open OBJ file %s", path.c_str());
			fclose(file);
		}

		return file;
	}

	// TODO: How do we properly signal failure here?
	void ObjParser::parseMTL(std::string& path)
	{
		FILE* file = openFile(path);

		if (!file)
		{
			return;
		}

		auto parseColor = [](auto& tokens) -> Vec3 {
			if (tokens.size() < 4)
			{
				Logger::error("Cannot parse color, not enough tokens.");
				assert(false);
			}

			return Vec3{ strToFloat(tokens[1]),
						 strToFloat(tokens[2]),
						 strToFloat(tokens[3]) };
		};

		MTL* mat = nullptr;

		const int MAX_LINE = 8192; // TODO: figure out proper value for max line length
		char input_line[MAX_LINE];
		char* result;
		std::string line;

		while ((result = fgets(input_line, MAX_LINE, file)) != nullptr)
		{
			result[strcspn(result, "\r\n")] = 0;
			trimTrailingWhitespace(result);
			auto tokens = tokenize(result, " ");
			char* firstToken = tokens[0];

			if (compare(firstToken, "newmtl"))
			{
				pMeshRaw->materials.push_back(MTL{});
				mat = &pMeshRaw->materials.back();
				mat->name = tokens[1];
			}
			else if (compare(firstToken, "Ka"))
			{
				mat->ambient = parseColor(tokens);
			}
			else if (compare(firstToken, "Kd"))
			{
				mat->diffuse = parseColor(tokens);
			}
			else if (compare(firstToken, "Ks"))
			{
				mat->specular = parseColor(tokens);
			}
			else if (compare(firstToken, "illum"))
			{
				mat->illum = static_cast<int8_t>(strToInt(tokens[1]));
			}
			else if (compare(firstToken, "Ns"))
			{
				mat->shininess = strToFloat(tokens[1]);
			}
			else if (compare(firstToken, "d") || compare(firstToken, "Tr"))
			{
				mat->transparency = strToFloat(tokens[1]);
			}
			else if (compare(firstToken, "map_Ka"))
			{
				mat->textureMap = tokens[1];
			}
		}

		fclose(file);
	}

	void ObjParser::selectFaceType(const char* faceToken)
	{
		if (find(faceToken, "/") == std::string::npos) // f a b c -> Vertex
		{
			vertexParser = &ObjParser::parseFaceVertexV;
			faceDelimiter = " ";
		}
		else if (find(faceToken, "//") != std::string::npos) // f a//u b//v c//w -> Vertex//Normal
		{
			vertexParser = &ObjParser::parseFaceVertexVN;
			faceDelimiter = "//";
		}
		else
		{
			std::string token = faceToken;
			std::ptrdiff_t sepCount = std::count(token.begin(), token.end(), '/');
			faceDelimiter = "/";

			if (sepCount == 1) // f a/i b/j c/k -> Vertex/Texture
			{
				vertexParser = &ObjParser::parseFaceVertexVT;
			}
			else // f a/i/u b/j/v c/k/w -> Vertex/Texture/Normal 
			{
				vertexParser = &ObjParser::parseFaceVertexVTN;
			}
		}
	}

	std::unique_ptr<Mesh> ObjParser::parseOBJ(const std::string& pathTo, const std::string& name)
	{
		std::string path = pathTo + name;
		FILE* file = openFile(path);

		if (!file)
		{
			return{};
		}

		auto mesh = std::make_unique<ObjData>();
		pMeshRaw = mesh.get();

		const int MAX_LINE = 8192; // TODO: figure out proper value for max line length
		char input_line[MAX_LINE];
		char* result;

		while ((result = fgets(input_line, MAX_LINE, file)) != nullptr)
		{
			result[strcspn(result, "\r\n")] = 0;
			trimTrailingWhitespace(result);
			auto tokens = tokenize(result, " ");
			char* firstToken = tokens[0];

			if (compare(firstToken, "v"))
			{
				parseVertex(tokens);
			}
			else if (compare(firstToken, "vn"))
			{
				parseNormal(tokens);
			}
			else if (compare(firstToken, "vt"))
			{
				parseUV(tokens);
			}
			else if (compare(firstToken, "f"))
			{
				if (vertexParser == nullptr)
				{
					selectFaceType(tokens[2]);
				}

				parseFace(tokens);
			}
			else if (compare(firstToken, "s"))
			{
				pMeshRaw->bSmoothShading = compare(tokens[1], "off");
			}
			else if (compare(firstToken, "mtllib"))
			{
				std::string fileName = tokens[1];
				parseMTL(pathTo + fileName);
			}
			else if (compare(firstToken, "usemtl"))
			{
				auto mat = std::find_if(pMeshRaw->materials.begin(), pMeshRaw->materials.end(),
					[name = tokens[1]](const MTL& mat) {
					return mat.name == name;
				});

				if (mat != pMeshRaw->materials.end())
				{
					if (pMeshRaw->faces.empty())
						mat->idxFaceFrom = 0;
					else
						mat->idxFaceFrom = pMeshRaw->faces.size() - 1;
				}
			}
		}

		ObjIndexer indexer;
		fclose(file);

		return indexer.convertForOpenGL(pMeshRaw);
	}

	std::unique_ptr<Mesh> loadObj(const std::string& pathTo, const std::string& name)
	{
		ObjParser parser;
		return parser.parseOBJ(pathTo, name);
	}
}
