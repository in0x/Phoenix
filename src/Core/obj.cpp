#include "obj.hpp"
#include <functional>
#include <cassert>
#include "StringTokenizer.hpp"
#include <map>

/*
	* TODO
	* Rearrange data so that we can actually use an index buffer -> DONE
	* Transform quad faces into two triangles -> DONE
	* Support parsing indices and values (f: 1.000/... vs f: -23/...) -> DONE
	* Make remapping run fast -> DONE
	* Handle normals / uvs being able to be missing -> DONE
	* Fix materials not being loaded (file is acting weird) -> DONE
	* Do one pass before hand to count sizes (should reduce loading time (less allocations))
*/

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
		std::unique_ptr<Mesh> convertForOpenGL(std::unique_ptr<ObjData> loaded)
		{
			const ObjData* pLoaded = loaded.get();

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
				(this->*toPacked)(packedVertices, face, pLoaded);

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

	private:
		void(ObjIndexer::*toPacked)(PackedVertexData*, const Face&, const ObjData*);
		void(ObjIndexer::*addData)(const PackedVertexData&);

		bool doesDataExist(const PackedVertexData& data, unsigned int& outIndex)
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

		void toPackedV(PackedVertexData* dataArr, const Face& face, const ObjData* loaded)
		{
			dataArr[0].vertex = loaded->vertices[face.vertexIndices[0]];
			dataArr[1].vertex = loaded->vertices[face.vertexIndices[1]];
			dataArr[2].vertex = loaded->vertices[face.vertexIndices[2]];
		}

		void toPackedVN(PackedVertexData* dataArr, const Face& face, const ObjData* loaded)
		{
			dataArr[0].vertex = loaded->vertices[face.vertexIndices[0]];
			dataArr[0].normal = loaded->normals[face.normalIndices[0]];

			dataArr[1].vertex = loaded->vertices[face.vertexIndices[1]];
			dataArr[1].normal = loaded->normals[face.normalIndices[1]];

			dataArr[2].vertex = loaded->vertices[face.vertexIndices[2]];
			dataArr[2].normal = loaded->normals[face.normalIndices[2]];
		}

		void toPackedVT(PackedVertexData* dataArr, const Face& face, const ObjData* loaded)
		{
			dataArr[0].vertex = loaded->vertices[face.vertexIndices[0]];
			dataArr[0].uv = loaded->uvs[face.uvIndices[0]];

			dataArr[1].vertex = loaded->vertices[face.vertexIndices[1]];
			dataArr[1].uv = loaded->uvs[face.uvIndices[1]];

			dataArr[2].vertex = loaded->vertices[face.vertexIndices[2]];
			dataArr[2].uv = loaded->uvs[face.uvIndices[2]];
		}

		void toPackedVTN(PackedVertexData* dataArr, const Face& face, const ObjData* loaded)
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

		void addV(const PackedVertexData& data)
		{
			pConvertedMesh->vertices.push_back(data.vertex);
		}
	
		void addVN(const PackedVertexData& data)
		{
			pConvertedMesh->vertices.push_back(data.vertex);
			pConvertedMesh->normals.push_back(data.normal);
		}

		void addVT(const PackedVertexData& data)
		{
			pConvertedMesh->vertices.push_back(data.vertex);
			pConvertedMesh->uvs.push_back(data.uv);
		}
	
		void addVTN(const PackedVertexData& data)
		{
			pConvertedMesh->vertices.push_back(data.vertex);
			pConvertedMesh->normals.push_back(data.normal);
			pConvertedMesh->uvs.push_back(data.uv);
		}
		
		void addDataAndIndex(const PackedVertexData& data)
		{
			(this->*addData)(data);

			unsigned int index = static_cast<unsigned int>(packed.size());
			packed[data] = index;
			pConvertedMesh->indices.push_back(index);
		}
	};

	// v -> Vertex(x, y, z, [w])
	void parseVertex(std::vector<char*>& tokens, ObjData* pScene)
	{
		if (tokens.size() < 4)
		{
			Logger::Error("Cannot parse vertex, not enough tokens.");
			assert(false);
		}
		
		pScene->vertices.push_back(Vec3{ strToFloat(tokens[1]),
										 strToFloat(tokens[2]), 
										 strToFloat(tokens[3]) });
	}

	// vn -> Vertex Normal(x, y, z)
	void parseNormal(std::vector<char*>& tokens, ObjData* pScene)
	{
		if (tokens.size() < 4)
		{
			Logger::Error("Cannot parse normal, not enough tokens.");
			assert(false);
		}

		pScene->normals.push_back(Vec3{ strToFloat(tokens[1]),
										strToFloat(tokens[2]),
										strToFloat(tokens[3]) });
	}

	// vt -> Texture Coord(u, v, [w])
	void parseUV(std::vector<char*>& tokens, ObjData* pScene)
	{
		if (tokens.size() < 3)
		{
			Logger::Error("Cannot parse uv, not enough tokens.");
			assert(false);
		}

		pScene->uvs.push_back(Vec2{ strToFloat(tokens[1]),
									strToFloat(tokens[2]) });
	}

	int mapFaceIndex(int index, size_t elementCount)
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

	void parseFaceVertexV(std::vector<char*>& tokens, int idx, ObjData* pScene)
	{
		pScene->faces.back().vertexIndices[idx] = mapFaceIndex(strToInt(tokens[0]), pScene->vertices.size());
	}

	void parseFaceVertexVN(std::vector<char*>& tokens, int idx, ObjData* pScene)
	{
		pScene->faces.back().vertexIndices[idx] = mapFaceIndex(strToInt(tokens[0]), pScene->vertices.size());
		pScene->faces.back().normalIndices[idx] = mapFaceIndex(strToInt(tokens[1]), pScene->normals.size());
	}

	void parseFaceVertexVT(std::vector<char*>& tokens, int idx, ObjData* pScene)
	{
		pScene->faces.back().vertexIndices[idx] = mapFaceIndex(strToInt(tokens[0]), pScene->vertices.size());
		pScene->faces.back().uvIndices[idx] = mapFaceIndex(strToInt(tokens[1]), pScene->uvs.size());
	}

	void parseFaceVertexVTN(std::vector<char*>& tokens, int idx, ObjData* pScene)
	{
		pScene->faces.back().vertexIndices[idx] = mapFaceIndex(strToInt(tokens[0]), pScene->vertices.size());
		pScene->faces.back().uvIndices[idx] = mapFaceIndex(strToInt(tokens[1]), pScene->uvs.size());
		pScene->faces.back().normalIndices[idx] = mapFaceIndex(strToInt(tokens[2]), pScene->normals.size());
	}

	/* f -> Face
	* Texture or normal can be missing
	*/
	void parseFace(std::vector<char*>& tokens, ObjData* pScene)
	{
		if (tokens.size() < 4)
		{
			Logger::Error("Cannot parse face, not enough tokens.");
			assert(false);
		}

		size_t tokenCount = tokens.size();
		size_t indexCount = tokenCount - 1;

		std::vector<std::vector<char*>> indices;
		indices.reserve(indexCount);
		char* delimiter = nullptr;

		// Deciding which parse function to use can probable be done once, check the spec if its consistent throught the entire file
		void(*vertexParser)(std::vector<char*>&, int, ObjData*) = nullptr; 

		if (find(tokens[2], "/") == std::string::npos) // f a b c -> Vertex
		{
			vertexParser = &parseFaceVertexV;
			delimiter = " ";
		}
		else if (find(tokens[2], "//") != std::string::npos) // f a//u b//v c//w -> Vertex//Normal
		{
			vertexParser = &parseFaceVertexVN;
			delimiter = "//";
		}
		else
		{
			std::string token = tokens[2];
			std::ptrdiff_t sepCount = std::count(token.begin(), token.end(), '/');
			delimiter = "/";

			if (sepCount == 1) // f a/i b/j c/k -> Vertex/Texture
			{
				vertexParser = &parseFaceVertexVT;
			}
			else // f a/i/u b/j/v c/k/w -> Vertex/Texture/Normal 
			{
				vertexParser = &parseFaceVertexVTN;
			}
		}

		for (size_t i = 1; i < tokenCount; ++i)
		{
			indices.push_back(tokenize(tokens[i], delimiter));
		}

		pScene->faces.emplace_back();
		vertexParser(indices[0], 0, pScene);
		vertexParser(indices[1], 1, pScene);
		vertexParser(indices[2], 2, pScene);

		for (size_t i = 3; i < indexCount; ++i)
		{
			pScene->faces.emplace_back();
			vertexParser(indices[0], 0, pScene);
			vertexParser(indices[i - 1], 1, pScene);
			vertexParser(indices[i], 2, pScene);
		}
	}

	void sanetizePath(std::string& path)
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

	FILE* openFile(std::string& path)
	{
		sanetizePath(path);
		FILE* file = fopen(path.c_str(), "r");

		if (!file)
		{
			Logger::Error("Failed to open OBJ file " + path);
			fclose(file);
		}

		return file;
	}

	// TODO: How do we properly signal failure here?
	void parseMTL(std::string& path, ObjData* pScene)
	{
		FILE* file = openFile(path);

		if (!file)
		{
			return;
		}

		auto parseColor = [](auto& tokens) -> Vec3 {
			if (tokens.size() < 4)
			{
				Logger::Error("Cannot parse color, not enough tokens.");
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
			auto tokens = tokenize(result, " ");
			const char* firstToken = tokens[0];

			if (compare(firstToken, "newmtl"))
			{
				pScene->materials.push_back(MTL{});
				mat = &pScene->materials.back();
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
				mat->illum = strToInt(tokens[1]);
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

	std::unique_ptr<Mesh> parseOBJ(const std::string& pathTo, const std::string& name)
	{
		std::string path = pathTo + name;
		FILE* file = openFile(path);

		if (!file)
		{
			return nullptr;
		}

		auto pScene = std::make_unique<ObjData>();
		ObjData* pSceneRaw = pScene.get();

		const int MAX_LINE = 8192; // TODO: figure out proper value for max line length
		char input_line[MAX_LINE];
		char* result;
		std::string line;

		while ((result = fgets(input_line, MAX_LINE, file)) != nullptr)
		{
			auto tokens = tokenize(result, " ");
			char* firstToken = tokens[0];

			if (compare(firstToken, "v"))
			{
				parseVertex(tokens, pSceneRaw);
			}
			else if (compare(firstToken, "vn"))
			{
				parseNormal(tokens, pSceneRaw);
			}
			else if (compare(firstToken, "vt"))
			{
				parseUV(tokens, pSceneRaw);
			}
			else if (compare(firstToken, "f"))
			{
				parseFace(tokens, pSceneRaw);
			}
			else if (compare(firstToken, "s"))
			{
				pScene->bSmoothShading = compare(tokens[1], "off");
			}
			else if (compare(firstToken, "mtllib"))
			{
				std::string fileName = tokens[1];
				parseMTL(pathTo + fileName, pSceneRaw);
			}
			else if (compare(firstToken, "usemtl"))
			{
				auto mat = std::find_if(pScene->materials.begin(), pScene->materials.end(),
					[name = tokens[1]](const MTL& mat) {
					return mat.name == name;
				});

				if (mat != pScene->materials.end())
				{
					if (pScene->faces.empty())
						mat->idxFaceFrom = 0;
					else
						mat->idxFaceFrom = pScene->faces.size() - 1;
				}
			}
		}

		ObjIndexer indexer;
		fclose(file);
		return indexer.convertForOpenGL(std::move(pScene));
		return{};
	}
}
