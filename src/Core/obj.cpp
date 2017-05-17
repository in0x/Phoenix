#include "obj.hpp"
#include <functional>
#include <cassert>

/*
	* TODO
	* Rearrange data so that we can actually use an index buffer -> DONE
	* Transform quad faces into two triangles -> DONE
	* Support parsing indices and values (f: 1.000/... vs f: -23/...)
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

	std::vector<std::string> strSplit(std::string& string, const char* pDelimiter)
	{
		int pos = 0;
		std::string token;
		std::string delimiter(pDelimiter);
		std::vector<std::string> elements{}; // Using a fixed size for this (i.e. 16), helps remove a lot of reallocations.

		while ((pos = string.find(delimiter)) != std::string::npos)
		{
			token = string.substr(0, pos);
			elements.push_back(token);
			string.erase(0, pos + delimiter.length());
		}

		elements.push_back(string);

		return elements;
	}

	// v -> Vertex(x, y, z, [w])
	void parseVertex(const std::vector<std::string>& tokens, ObjData* pScene)
	{
		if (tokens.size() < 4)
		{
			Logger::Error("Cannot parse vertex, not enough tokens.");
			assert(false);
		}

		pScene->vertices.push_back(Vec3{ std::stof(tokens[1]),
			std::stof(tokens[2]),
			std::stof(tokens[3]) });
	}

	// vn -> Vertex Normal(x, y, z)
	void parseNormal(const std::vector<std::string>& tokens, ObjData* pScene)
	{
		if (tokens.size() < 4)
		{
			Logger::Error("Cannot parse normal, not enough tokens.");
			assert(false);
		}

		pScene->normals.push_back(Vec3{ std::stof(tokens[1]),
			std::stof(tokens[2]),
			std::stof(tokens[3]) });
	}

	// vt -> Texture Coord(u, v, [w])
	void parseUV(const std::vector<std::string>& tokens, ObjData* pScene)
	{
		if (tokens.size() < 3)
		{
			Logger::Error("Cannot parse uv, not enough tokens.");
			assert(false);
		}

		pScene->uvs.push_back(Vec2{ std::stof(tokens[1]),
			std::stof(tokens[2]) });
	}

	float parseFaceIndex(std::string& token, size_t elementCount)
	{
		float index = std::stof(token); // TODO: These should really be converted to int

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

	/* f -> Face
	* Texture or normal can be missing
	*/
	void parseFace(std::vector<std::string>& tokens, ObjData* pScene)
	{
		if (tokens.size() < 4)
		{
			Logger::Error("Cannot parse face, not enough tokens.");
			assert(false);
		}

		std::function<void(std::string&, int)> vertexParser;

		if (tokens[2].find("/") == std::string::npos) // f a b c -> Vertex
		{
			vertexParser = [pScene](auto token, int idx)
			{
				pScene->faces.back().vertexIndices(idx) = parseFaceIndex(token, pScene->vertices.size());
			};
		}
		else if (tokens[2].find("//") != std::string::npos) // f a//u b//v c//w -> Vertex//Normal
		{
			vertexParser = [pScene](auto token, int idx)
			{
				auto nums = strSplit(token, "//");
				pScene->faces.back().vertexIndices(idx) = parseFaceIndex(nums[0], pScene->vertices.size());
				pScene->faces.back().normalIndices(idx) = parseFaceIndex(nums[1], pScene->normals.size());
			};
		}
		else
		{
			int sepCount = std::count(tokens[2].begin(), tokens[2].end(), '/');

			if (sepCount == 1) // f a/i b/j c/k -> Vertex/Texture
			{
				vertexParser = [pScene](auto token, int idx)
				{
					auto nums = strSplit(token, "/");
					pScene->faces.back().vertexIndices(idx) = parseFaceIndex(nums[0], pScene->vertices.size());
					pScene->faces.back().uvIndices(idx) = parseFaceIndex(nums[1], pScene->uvs.size());
				};
			}
			else // f a/i/u b/j/v c/k/w -> Vertex/Texture/Normal 
			{
				vertexParser = [&](auto token, int idx)
				{
					auto nums = strSplit(token, "/");
					pScene->faces.back().vertexIndices(idx) = parseFaceIndex(nums[0], pScene->vertices.size());
					pScene->faces.back().uvIndices(idx) = parseFaceIndex(nums[1], pScene->uvs.size());
					pScene->faces.back().normalIndices(idx) = parseFaceIndex(nums[2], pScene->normals.size());
				};
			}
		}

		pScene->faces.push_back(Face{});
		vertexParser(tokens[1], 0);
		vertexParser(tokens[2], 1);
		vertexParser(tokens[3], 2);

		size_t tokenCount = tokens.size();
		
		if (tokenCount > 4)
		{
			for (size_t i = 4; i < tokenCount; ++i)
			{
				pScene->faces.push_back(Face{});
				vertexParser(tokens[i - 3], 0);
				vertexParser(tokens[i - 1], 1);
				vertexParser(tokens[i],		2);
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
		std::ifstream file = openFile(path);

		if (!file)
			return;

		std::string line;

		auto parseColor = [](auto& tokens) -> Vec3 {
			if (tokens.size() < 4)
			{
				Logger::Error("Cannot parse color, not enough tokens.");
				assert(false);
			}

			return Vec3{ std::stof(tokens[1]),
						 std::stof(tokens[2]),
						 std::stof(tokens[3])};
		};

		Material* mat = nullptr;

		while (std::getline(file, line))
		{
			auto tokens = strSplit(line, " ");

			if (tokens[0] == "newmtl")
			{
				pScene->materials.push_back(Material{});
				mat = &pScene->materials.back();
				mat->name = tokens[1];
			}
			else if (tokens[0] == "Ka")
			{
				mat->ambient = parseColor(tokens);
			}
			else if (tokens[0] == "Kd")
			{
				mat->diffuse = parseColor(tokens);
			}
			else if (tokens[0] == "Ks")
			{
				mat->specular = parseColor(tokens);
			}
			else if (tokens[0] == "illum")
			{
				mat->illum = std::stof(tokens[1]);
			}
			else if (tokens[0] == "Ns")
			{
				mat->shininess = std::stof(tokens[1]);
			}
			else if (tokens[0] == "d" || tokens[0] == "Tr")
			{
				mat->transperency = std::stof(tokens[1]);
			}
			else if (tokens[0] == "map_Ka")
			{
				mat->textureMap = tokens[1];
			}
		}
	}

	std::unique_ptr<Mesh> parseOBJ(const std::string& pathTo, const std::string& name)
	{
		auto file = openFile(pathTo + name);

		if (!file)
			return nullptr;

		auto pScene = std::make_unique<ObjData>();
		ObjData* pSceneRaw = pScene.get();
		std::string line;

		while (std::getline(file, line))
		{
			auto tokens = strSplit(line, " ");

			if (tokens[0] == "v")
			{
				parseVertex(tokens, pSceneRaw);
			}
			else if (tokens[0] == "vn")
			{
				parseNormal(tokens, pSceneRaw);
			}
			else if (tokens[0] == "vt")
			{
				parseUV(tokens, pSceneRaw);
			}
			else if (tokens[0] == "f")
			{
				parseFace(tokens, pSceneRaw);
			}
			else if (tokens[0] == "s")
			{
				pScene->bSmoothShading = tokens[1] == "off";
			}
			else if (tokens[0] == "mtllib")
			{
				parseMTL(pathTo + tokens[1], pSceneRaw);
			}
			else if (tokens[0] == "usemtl")
			{
				auto mat = std::find_if(pScene->materials.begin(), pScene->materials.end(),
					[&name = tokens[1]](const Material& mat) {
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