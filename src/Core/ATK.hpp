#pragma once

/*
	Small library for loading Asset files. This will ony support .obj
	at first since most of the models I own are in that format.
*/

/*
	TODO:
		* Figure out what obj and mtl can contain
		* Figure out data representations
		* Figure out how to do parsing efficiently

	Links:
		* http://web.cse.ohio-state.edu/~hwshen/581/Site/Lab3_files/Labhelp_Obj_parser.htm

	OBJ:
		* # -> Comment

		* mtllib [*.mtl] -> Include an .mtl file containing materials

		* o [*] -> object name (TODO: What is this used for?)

		* g [*] -> group name (TODO: What is this used for?)

		* usemtl [*] -> start using a material declared in an mtl file

		* s -> Smooth shading (TODO: What is this used for ?)

	MTL:
		* newmtl -> Define a new Material

		* Ka -> Ambient Color

		* Kd -> Diffuse Color

		* Ks -> Specular Color

		* Illum -> Illumination Type
			* Can be:
				* 1: No specular shading
				* 2: Specular shading

		* Ns: Specular Exponent

		* [d|Tr] -> Transperency (TODO: Are both of these used?)

		* map_Ka -> Name of a Texture map containing RGB values
*/

#include "PhiCoreRequired.hpp"
#include "Logger.hpp"

#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>
#include <functional>

#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec4.hpp"

namespace Phoenix::ATK
{
	struct Material
	{
		std::string name;
		std::string TextureMap;
		f32 Ambient;
		f32 Diffuse;
		f32 Specular;
		int8 Illum;
		f32 Shininess;
		f32 Transperency;
	};

	struct Face
	{
		Vec3 vertexIndices;
		Vec3 normalIndices;
		Vec3 uvIndices;
	};

	struct Scene
	{
		std::vector<Vec3> vertices;
		std::vector<Vec2> uvs;
		std::vector<Vec3> normals;
		std::vector<Face> faces;
		std::vector<Material> materials;
	};

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
	void parseVertex(const std::vector<std::string>& tokens, Scene* pScene)
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
	void parseNormal(const std::vector<std::string>& tokens, Scene* pScene)
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
	void parseUV(const std::vector<std::string>& tokens, Scene* pScene)
	{
		if (tokens.size() < 3)
		{
			Logger::Error("Cannot parse uv, not enough tokens.");
			assert(false);
		}

		pScene->uvs.push_back(Vec2{ std::stof(tokens[1]),
			std::stof(tokens[2]) });
	}

	/* f-> Face
	 * Texture or normal can be missing
	 */
	void parseFace(std::vector<std::string>& tokens, Scene* pScene)
	{
		std::function<void(std::string&, int)> parser;
		pScene->faces.push_back(Face{});

		if (tokens[2].find("/") == std::string::npos) // f a b c -> Vertex
		{
			parser = [pScene](auto token, int idx) 
			{
				pScene->faces.back().vertexIndices(idx) = std::stof(token);
			};
		}
		else if (tokens[2].find("//")) // f a//u b//v c//w -> Vertex//Normal
		{
			parser = [pScene](auto token, int idx)
			{
				auto nums = strSplit(token, "//");
				pScene->faces.back().vertexIndices(idx) = std::stof(nums[0]);
				pScene->faces.back().normalIndices(idx) = std::stof(nums[1]);
			};
		}
		else
		{
			auto sepCount = tokens[2].find("/");

			if (sepCount == 1) // f a/i b/j c/k -> Vertex/Texture
			{
				parser = [pScene](auto token, int idx) 
				{
					auto nums = strSplit(token, "/");
					pScene->faces.back().vertexIndices(idx) = std::stof(std::string{ nums[0] });
					pScene->faces.back().uvIndices(idx) = std::stof(std::string{ nums[1] });
				};
			}
			else // f a/i/u b/j/v c/k/w -> Vertex/Texture/Normal 
			{
				parser = [&](auto token, int idx) 
				{
					auto nums = strSplit(token, "/");
					pScene->faces.back().vertexIndices(idx) = std::stof(std::string{ nums[0] });
					pScene->faces.back().uvIndices(idx) = std::stof(std::string{ nums[1] });
					pScene->faces.back().normalIndices(idx) = std::stof(std::string{ nums[2] });
				};
			}
		}

		parser(tokens[1], 0);
		parser(tokens[2], 1);
		parser(tokens[3], 2);
	}

	std::unique_ptr<Material> parseMTL(const std::string& path)
	{
		return std::make_unique<Material>();
	}

	std::unique_ptr<Scene> parseOBJ(const std::string& path)
	{
		auto pScene = std::make_unique<Scene>();

		std::ifstream file;
		file.open(path);

		if (!file.good())
		{
			Logger::Error("Failed to open OBJ file " + path);
			return nullptr;
		}

		Scene* pSceneRaw = pScene.get();
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
		}

		return pScene;
	}
};

