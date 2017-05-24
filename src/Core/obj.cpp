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
	* Handle normals / uvs being able to be missing
*/

namespace Phoenix
{
	struct ObjData
	{
		std::vector<Vec3> vertices;
		std::vector<Vec2> uvs;
		std::vector<Vec3> normals;
		std::vector<Face> faces;
		std::vector<Material> materials;
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

		std::map<PackedVertexData, size_t> packed;
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
					size_t index;
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
		//std::function<void(PackedVertexData* dataArr, const Face& face, const ObjData* loaded)> toPacked;
		//std::function<void(const PackedVertexData&)> addData;

		void(ObjIndexer::*toPacked)(PackedVertexData*, const Face&, const ObjData*);
		void(ObjIndexer::*addData)(const PackedVertexData&);

		bool doesDataExist(const PackedVertexData& data, size_t& outIndex)
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
			dataArr[0].vertex = loaded->vertices[face.vertexIndices.x];
			dataArr[1].vertex = loaded->vertices[face.vertexIndices.y];
			dataArr[2].vertex = loaded->vertices[face.vertexIndices.z];
		}

		void toPackedVN(PackedVertexData* dataArr, const Face& face, const ObjData* loaded)
		{
			dataArr[0].vertex = loaded->vertices[face.vertexIndices.x];
			dataArr[0].normal = loaded->normals[face.normalIndices.x];

			dataArr[1].vertex = loaded->vertices[face.vertexIndices.y];
			dataArr[1].normal = loaded->normals[face.normalIndices.y];

			dataArr[2].vertex = loaded->vertices[face.vertexIndices.z];
			dataArr[2].normal = loaded->normals[face.normalIndices.z];
		}

		void toPackedVT(PackedVertexData* dataArr, const Face& face, const ObjData* loaded)
		{
			dataArr[0].vertex = loaded->vertices[face.vertexIndices.x];
			dataArr[0].uv = loaded->uvs[face.uvIndices.x];

			dataArr[1].vertex = loaded->vertices[face.vertexIndices.y];
			dataArr[1].uv = loaded->uvs[face.uvIndices.y];

			dataArr[2].vertex = loaded->vertices[face.vertexIndices.z];
			dataArr[2].uv = loaded->uvs[face.uvIndices.z];
		}

		void toPackedVTN(PackedVertexData* dataArr, const Face& face, const ObjData* loaded)
		{
			dataArr[0].vertex = loaded->vertices[face.vertexIndices.x];
			dataArr[0].normal = loaded->normals[face.normalIndices.x];
			dataArr[0].uv = loaded->uvs[face.uvIndices.x];

			dataArr[1].vertex = loaded->vertices[face.vertexIndices.y];
			dataArr[1].normal = loaded->normals[face.normalIndices.y];
			dataArr[1].uv = loaded->uvs[face.uvIndices.y];

			dataArr[2].vertex = loaded->vertices[face.vertexIndices.z];
			dataArr[2].normal = loaded->normals[face.normalIndices.z];
			dataArr[2].uv = loaded->uvs[face.uvIndices.z];
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

			size_t index = packed.size();
			packed[data] = index;
			pConvertedMesh->indices.push_back(index);
		}
	};

	// v -> Vertex(x, y, z, [w])
	void parseVertex(StringTokenizer& tokens, ObjData* pScene)
	{
		if (tokens.size() < 4)
		{
			Logger::Error("Cannot parse vertex, not enough tokens.");
			assert(false);
		}

		pScene->vertices.push_back(Vec3{ tokens.tokenToFloat(1),
										 tokens.tokenToFloat(2),
										 tokens.tokenToFloat(3) });
	}

	// vn -> Vertex Normal(x, y, z)
	void parseNormal(StringTokenizer& tokens, ObjData* pScene)
	{
		if (tokens.size() < 4)
		{
			Logger::Error("Cannot parse normal, not enough tokens.");
			assert(false);
		}

		pScene->normals.push_back(Vec3{ tokens.tokenToFloat(1),
										tokens.tokenToFloat(2),
										tokens.tokenToFloat(3) });
	}

	// vt -> Texture Coord(u, v, [w])
	void parseUV(StringTokenizer& tokens, ObjData* pScene)
	{
		if (tokens.size() < 3)
		{
			Logger::Error("Cannot parse uv, not enough tokens.");
			assert(false);
		}

		pScene->uvs.push_back(Vec2{ tokens.tokenToFloat(1),
									tokens.tokenToFloat(2) });
	}

	float mapFaceIndex(float index, size_t elementCount)
	{
		if (index < 0)
		{
			index = elementCount + index;
		}
		else
		{
			// Subtract 1 from indices because obj is 1-based.
			index -= 1;
		}

		return index;
	}

	void parseFaceVertexV(const std::string& token, int idx, ObjData* pScene)
	{
		pScene->faces.back().vertexIndices(idx) = mapFaceIndex(strToFloat(token.c_str()), pScene->vertices.size());
	}

	void parseFaceVertexVN(const std::string& token, int idx, ObjData* pScene)
	{
		StringTokenizer nums = StringTokenizer(token, "//");
		pScene->faces.back().vertexIndices(idx) = mapFaceIndex(nums.tokenToFloat(0), pScene->vertices.size());
		pScene->faces.back().normalIndices(idx) = mapFaceIndex(nums.tokenToFloat(1), pScene->normals.size());
	}

	void parseFaceVertexVT(const std::string& token, int idx, ObjData* pScene)
	{
		StringTokenizer nums = StringTokenizer(token, "/");
		pScene->faces.back().vertexIndices(idx) = mapFaceIndex(nums.tokenToFloat(0), pScene->vertices.size());
		pScene->faces.back().uvIndices(idx) = mapFaceIndex(nums.tokenToFloat(1), pScene->uvs.size());
	}

	void parseFaceVertexVTN(const std::string& token, int idx, ObjData* pScene)
	{
		StringTokenizer nums = StringTokenizer(token, "/");
		pScene->faces.back().vertexIndices(idx) = mapFaceIndex(nums.tokenToFloat(0), pScene->vertices.size());
		pScene->faces.back().uvIndices(idx) = mapFaceIndex(nums.tokenToFloat(1), pScene->uvs.size());
		pScene->faces.back().normalIndices(idx) = mapFaceIndex(nums.tokenToFloat(2), pScene->normals.size());
	}

	/* f -> Face
	* Texture or normal can be missing
	*/
	void parseFace(StringTokenizer& tokens, ObjData* pScene)
	{
		if (tokens.size() < 4)
		{
			Logger::Error("Cannot parse face, not enough tokens.");
			assert(false);
		}

		void(*vertexParser)(const std::string&, int, ObjData*) = nullptr;

		if (!tokens.find(2, "/")) // f a b c -> Vertex
		{
			vertexParser = &parseFaceVertexV;
		}
		else if (tokens.find(2, "//")) // f a//u b//v c//w -> Vertex//Normal
		{
			vertexParser = &parseFaceVertexVN;
		}
		else
		{
			std::string token = tokens[2];
			int sepCount = std::count(token.begin(), token.end(), '/');

			if (sepCount == 1) // f a/i b/j c/k -> Vertex/Texture
			{
				vertexParser = &parseFaceVertexVT;
			}
			else // f a/i/u b/j/v c/k/w -> Vertex/Texture/Normal 
			{
				vertexParser = &parseFaceVertexVTN;
			}
		}

		pScene->faces.push_back(Face{});
		vertexParser(tokens[1], 0, pScene);
		vertexParser(tokens[2], 1, pScene);
		vertexParser(tokens[3], 2, pScene);

		size_t tokenCount = tokens.size();

		if (tokenCount > 4)
		{
			for (size_t i = 4; i < tokenCount; ++i)
			{
				pScene->faces.push_back(Face{});
				vertexParser(tokens[1], 0, pScene);
				vertexParser(tokens[i - 1], 1, pScene);
				vertexParser(tokens[i], 2, pScene);
			}
		}
	}

	std::ifstream openFile(const std::string& path)
	{
		std::ifstream file;
		file.open(path);

		if (!file.good())
		{
			Logger::Error("Failed to open OBJ file " + path);
			return file;
		}

		return file;
	}

	// How do we properly signal failure here?
	void parseMTL(const std::string& path, ObjData* pScene)
	{
		FILE* file = fopen(path.c_str(), "r");

		if (!file)
			return;

		auto parseColor = [](auto& tokens) -> Vec3 {
			if (tokens.size() < 4)
			{
				Logger::Error("Cannot parse color, not enough tokens.");
				assert(false);
			}

			return Vec3{ tokens.tokenToFloat(1),
						 tokens.tokenToFloat(2),
						 tokens.tokenToFloat(3) };
		};

		Material* mat = nullptr;

		const int MAX_LINE = 8192; // TODO: figure out proper value for max line length
		char input_line[MAX_LINE];
		char* result;
		std::string line;

		while ((result = fgets(input_line, MAX_LINE, stdin)) != NULL)
		{
			line = result;
			StringTokenizer tokens = StringTokenizer(line, " ");

			if (tokens.compare(0, "newmtl"))
			{
				pScene->materials.push_back(Material{});
				mat = &pScene->materials.back();
				mat->name = tokens[1];
			}
			else if (tokens.compare(0, "Ka"))
			{
				mat->ambient = parseColor(tokens);
			}
			else if (tokens.compare(0, "Kd"))
			{
				mat->diffuse = parseColor(tokens);
			}
			else if (tokens.compare(0, "Ks"))
			{
				mat->specular = parseColor(tokens);
			}
			else if (tokens.compare(0, "illum"))
			{
				mat->illum = tokens.tokenToFloat(1);
			}
			else if (tokens.compare(0, "Ns"))
			{
				mat->shininess = tokens.tokenToFloat(1);
			}
			else if (tokens.compare(0, "d") || tokens.compare(0, "Tr"))
			{
				mat->transperency = tokens.tokenToFloat(1);
			}
			else if (tokens.compare(0, "map_Ka"))
			{
				mat->textureMap = tokens[1];
			}
		}
	}

	std::unique_ptr<Mesh> parseOBJ(const std::string& pathTo, const std::string& name)
	{
		FILE* file = fopen((pathTo + name).c_str(), "r");

		if (!file)
			return nullptr;

		auto pScene = std::make_unique<ObjData>();
		ObjData* pSceneRaw = pScene.get();

		const int MAX_LINE = 8192; // TODO: figure out proper value for max line length
		char input_line[MAX_LINE];
		char* result;
		std::string line;

		while ((result = fgets(input_line, MAX_LINE, file)) != NULL)
		{
			line = result;
			StringTokenizer tokens = StringTokenizer(line, " ");

			if (tokens.compare(0, "v"))
			{
				parseVertex(tokens, pSceneRaw);
			}
			else if (tokens.compare(0, "vn"))
			{
				parseNormal(tokens, pSceneRaw);
			}
			else if (tokens.compare(0, "vt"))
			{
				parseUV(tokens, pSceneRaw);
			}
			else if (tokens.compare(0, "f"))
			{
				parseFace(tokens, pSceneRaw);
			}
			else if (tokens.compare(0, "s"))
			{
				pScene->bSmoothShading = tokens.compare(1, "off");
			}
			else if (tokens.compare(0, "mtllib"))
			{
				parseMTL(pathTo + tokens[1], pSceneRaw);
			}
			else if (tokens.compare(0, "usemtl"))
			{
				auto mat = std::find_if(pScene->materials.begin(), pScene->materials.end(),
					[name = tokens[1]](const Material& mat) {
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
		return indexer.convertForOpenGL(std::move(pScene));
	}
}
