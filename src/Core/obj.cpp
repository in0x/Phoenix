#include "obj.hpp"
#include <functional>
#include <cassert>

/*
	* TODO
	* Rearrange data so that we can actually use an index buffer
	* Transform quad faces into two triangles
	* Support parsing indices and values (f: 1.000/... vs f: -23/...)
*/

namespace Phoenix
{
	std::vector<std::string> strSplit(std::string& string, const char* pDelimiter)
	{
		int pos = 0;
		std::string token;
		std::string delimiter(pDelimiter);
		std::vector<std::string> elements{};

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
	void parseVertex(const std::vector<std::string>& tokens, Mesh* pScene)
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
	void parseNormal(const std::vector<std::string>& tokens, Mesh* pScene)
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
	void parseUV(const std::vector<std::string>& tokens, Mesh* pScene)
	{
		if (tokens.size() < 3)
		{
			Logger::Error("Cannot parse uv, not enough tokens.");
			assert(false);
		}

		pScene->uvs.push_back(Vec2{ std::stof(tokens[1]),
			std::stof(tokens[2]) });
	}

	/* f -> Face
	* Texture or normal can be missing
	*/
	void parseFace(std::vector<std::string>& tokens, Mesh* pScene)
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
				pScene->faces.back().vertexIndices(idx) = std::stof(token);
			};
		}
		else if (tokens[2].find("//")) // f a//u b//v c//w -> Vertex//Normal
		{
			vertexParser = [pScene](auto token, int idx)
			{
				auto nums = strSplit(token, "//");
				pScene->faces.back().vertexIndices(idx) = std::stof(nums[0]);
				pScene->faces.back().normalIndices(idx) = std::stof(nums[1]);
			};
		}
		else
		{
			size_t sepCount = tokens[2].find("/");

			if (sepCount == 1) // f a/i b/j c/k -> Vertex/Texture
			{
				vertexParser = [pScene](auto token, int idx)
				{
					auto nums = strSplit(token, "/");
					pScene->faces.back().vertexIndices(idx) = std::stof(std::string{ nums[0] });
					pScene->faces.back().uvIndices(idx) = std::stof(std::string{ nums[1] });
				};
			}
			else // f a/i/u b/j/v c/k/w -> Vertex/Texture/Normal 
			{
				vertexParser = [&](auto token, int idx)
				{
					auto nums = strSplit(token, "/");
					pScene->faces.back().vertexIndices(idx) = std::stof(std::string{ nums[0] });
					pScene->faces.back().uvIndices(idx) = std::stof(std::string{ nums[1] });
					pScene->faces.back().normalIndices(idx) = std::stof(std::string{ nums[2] });
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
	void parseMTL(const std::string& path, Mesh* pScene)
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

	std::unique_ptr<Mesh> convert(const Mesh* loaded)
	{
		auto converted = std::make_unique<Mesh>;
		return nullptr;
	}

	std::unique_ptr<Mesh> parseOBJ(const std::string& pathTo, const std::string& name)
	{
		auto file = openFile(pathTo + name);

		if (!file)
			return nullptr;

		auto pScene = std::make_unique<Mesh>();
		Mesh* pSceneRaw = pScene.get();
		std::string line;

		while (std::getline(file, line))
		{
		
		}

		file.clear();
		file.seekg(0, std::ios::beg);

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
				if (tokens[1] == "off")
					pScene->bSmoothShading = 0;
				else
					pScene->bSmoothShading = 1;
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

		//return convert(pScene.get());
		return pScene;
	}
}
