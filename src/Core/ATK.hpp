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

		* v -> Vertex (x,y,z,[w])

		* vt -> Texture Coord (u,v,[w])
		
		* vn -> Vertex Normal (x,y,z)
		
		* f -> Face
			* f a/i/u b/j/v c/k/w -> Vertex / Texture / Normal
			* Texture or normal can be missing
				* f a/i b/j c/k -> Vertex / Texture
				* f a//u b//v c//w -> Vertex / / Normal

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

	};

	struct Scene
	{
		std::vector<Vec3> vertices;
		std::vector<Vec2> textures;
		std::vector<Vec3> normals;
		std::vector<Material> materials;
	};

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

		return pScene;
	}

	std::unique_ptr<Material> parseMTL(const std::string& path)
	{
		return std::make_unique<Material>();
	}	
};

