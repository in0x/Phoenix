#include "obj.hpp"
#include <functional>
#include <cassert>
#include "StringTokenizer.hpp"

/*
	* TODO
	* Rearrange data so that we can actually use an index buffer -> DONE
	* Transform quad faces into two triangles -> DONE
	* Support parsing indices and values (f: 1.000/... vs f: -23/...)
*/

namespace Phoenix
{
	float strToFloat(const std::string& s)
	{
		const char* string = s.c_str();

		float real = 0.0f;
		bool neg = false;
		if (*string == '-') 
		{
			neg = true;
			++string;
		}

		while (*string >= '0' && *string <= '9') 
		{
			real = (real * 10.0f) + (*string - '0');
			++string;
		}
		
		if (*string == '.') 
		{
			float fract = 0.0f;
			int fractLen = 0;
			++string;

			while (*string >= '0' && *string <= '9') 
			{
				fract = (fract*10.0f) + (*string - '0');
				++string;
				++fractLen;
			}
			
			real += fract / std::pow(10.0f, fractLen);
		}
		if (neg) 
		{
			real = -real;
		}
		return real;
	}

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
			int index;
		};

		std::vector<PackedVertexData> packed;
		std::unique_ptr<Mesh> pConvertedMesh;

	public:
		std::unique_ptr<Mesh> convertForOpenGL(std::unique_ptr<ObjData> loaded)
		{
			pConvertedMesh = std::make_unique<Mesh>();

			pConvertedMesh->materials.swap(loaded->materials);
			pConvertedMesh->bSmoothShading = loaded->bSmoothShading;

			size_t vertexCount = pConvertedMesh->vertices.size();
			pConvertedMesh->vertices.reserve(vertexCount);
			pConvertedMesh->normals.reserve(vertexCount);
			pConvertedMesh->uvs.reserve(vertexCount);

			packed.resize(vertexCount);

			for (const Face& face : loaded->faces)
			{
				PackedVertexData packedVertices[] = {
					{
						loaded->vertices[face.vertexIndices.x],
						loaded->normals[face.normalIndices.x],
						loaded->uvs[face.uvIndices.x]
					},
					{
						loaded->vertices[face.vertexIndices.y],
						loaded->normals[face.normalIndices.y],
						loaded->uvs[face.uvIndices.y]
					},
					{
						loaded->vertices[face.vertexIndices.z],
						loaded->normals[face.normalIndices.z],
						loaded->uvs[face.uvIndices.z]
					}
				};

				for (const auto& data : packedVertices)
				{
					int index;
					bool bDataExists = doesDataExist(data, index);

					if (bDataExists)
					{
						pConvertedMesh->indices.push_back(index);
					}
					else
					{
						addData(data);
					}
				}
			}

			return std::move(pConvertedMesh);
		}

	private:
		bool doesDataExist(const PackedVertexData& data, int& outIndex)
		{
			auto iter = std::find_if(packed.begin(), packed.end(), [&](const PackedVertexData& lhv)
			{
				return lhv.vertex == data.vertex && lhv.normal == data.normal && lhv.uv == data.uv;
			});

			if (iter == packed.end())
			{
				return false;
			}
			else
			{
				outIndex = iter->index;
				return true;
			}
		}

		void addData(const PackedVertexData& data)
		{
			pConvertedMesh->vertices.push_back(data.vertex);
			pConvertedMesh->normals.push_back(data.normal);
			pConvertedMesh->uvs.push_back(data.uv);

			packed.push_back(data);
			packed.back().index = packed.size() - 1;
			pConvertedMesh->indices.push_back(packed.back().index);
		}
	};

	//int subStrCount(const std::string& string, const std::string& substr)
	//{
	//	int count = 0;
	//	size_t pos = 0;
	//	while ((pos = string.find(substr, pos)) != std::string::npos)
	//	{
	//		++count;
	//		++pos;
	//	}
	//	return count; 
	//}

	//std::vector<std::string> strSplit(std::string& string, const char* pDelimiter)
	//{
	//	int pos = 0;
	//	std::string token;
	//	std::string delimiter(pDelimiter);

	//	int sepCount = subStrCount(string, delimiter);
	//	std::vector<std::string> elements;
	//	elements.reserve(sepCount + 1);

	//	while ((pos = string.find(delimiter)) != std::string::npos)
	//	{
	//		token = string.substr(0, pos);
	//		elements.emplace_back(token);
	//		string.erase(0, pos + delimiter.length());
	//	}

	//	elements.push_back(string);

	//	return elements;
	//}

	// v -> Vertex(x, y, z, [w])
	void parseVertex(StringTokenizer& tokens, ObjData* pScene)
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
	void parseNormal(StringTokenizer& tokens, ObjData* pScene)
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
	void parseUV(StringTokenizer& tokens, ObjData* pScene)
	{
		if (tokens.size() < 3)
		{
			Logger::Error("Cannot parse uv, not enough tokens.");
			assert(false);
		}

		pScene->uvs.push_back(Vec2{ strToFloat(tokens[1]),
			strToFloat(tokens[2]) });
	}

	float parseFaceIndex(std::string& token, size_t elementCount)
	{
		float index = strToFloat(token); // TODO: These should really be converted to int

		if (index < 0)
		{
			index = elementCount - index;
		}
		else
		{
			// Subtract 1 from indices because obj is 1-based.
			index -= 1;
		}
		
		return index;
	}

	void parseFaceVertexV(std::string& token, int idx, ObjData* pScene)
	{
		pScene->faces.back().vertexIndices(idx) = parseFaceIndex(token, pScene->vertices.size());
	}

	void parseFaceVertexVN(std::string& token, int idx, ObjData* pScene)
	{
		StringTokenizer nums = StringTokenizer(token, "//");
		pScene->faces.back().vertexIndices(idx) = parseFaceIndex(nums[0], pScene->vertices.size());
		pScene->faces.back().normalIndices(idx) = parseFaceIndex(nums[1], pScene->normals.size());
	}

	void parseFaceVertexVT(std::string& token, int idx, ObjData* pScene)
	{
		StringTokenizer nums = StringTokenizer(token, "/");
		pScene->faces.back().vertexIndices(idx) = parseFaceIndex(nums[0], pScene->vertices.size());
		pScene->faces.back().uvIndices(idx) = parseFaceIndex(nums[1], pScene->uvs.size());
	}
		
	void parseFaceVertexVTN(std::string& token, int idx, ObjData* pScene)
	{
		StringTokenizer nums = StringTokenizer(token, "/");
		pScene->faces.back().vertexIndices(idx) = parseFaceIndex(nums[0], pScene->vertices.size());
		pScene->faces.back().uvIndices(idx) = parseFaceIndex(nums[1], pScene->uvs.size());
		pScene->faces.back().normalIndices(idx) = parseFaceIndex(nums[2], pScene->normals.size());
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

		void (*vertexParser)(std::string&, int, ObjData*) = nullptr;
		
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
				vertexParser(tokens[i - 3], 0, pScene);
				vertexParser(tokens[i - 1], 1, pScene);
				vertexParser(tokens[i],		2, pScene);
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
		//std::ifstream file = openFile(path);
		FILE* file = fopen(path.c_str(), "r");

		if (!file)
			return;

		auto parseColor = [](auto& tokens) -> Vec3 {
			if (tokens.size() < 4)
			{
				Logger::Error("Cannot parse color, not enough tokens.");
				assert(false);
			}

			return Vec3{ strToFloat(tokens[1]),
						 strToFloat(tokens[2]),
						 strToFloat(tokens[3])};
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
				mat->illum = strToFloat(tokens[1]);
			}
			else if (tokens.compare(0, "Ns"))
			{
				mat->shininess = strToFloat(tokens[1]);
			}
			else if (tokens.compare(0, "d") || tokens.compare(0, "Tr"))
			{
				mat->transperency = strToFloat(tokens[1]);
			}
			else if (tokens.compare(0, "map_Ka"))
			{
				mat->textureMap = tokens[1];
			}
		}
	}

	std::unique_ptr<Mesh> parseOBJ(const std::string& pathTo, const std::string& name)
	{
		//auto file = openFile(pathTo + name);
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
		pScene->uvs.push_back({}); // TODO: Remove this, this is a super temporary fix to avoid having to work around missing uvs for now.
		return indexer.convertForOpenGL(std::move(pScene));
	}
}
