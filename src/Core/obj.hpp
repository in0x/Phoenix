#pragma once

/*
	For loading Asset files. This will ony support .obj
	at first since most of the models I own are in that format.
*/

#include "Logger.hpp"
#include <fstream>
#include <vector>
#include <memory>
#include "Math/PhiMath.hpp"

namespace Phoenix
{
	struct Face
	{
		Vec3 vertexIndices;
		Vec3 normalIndices;
		Vec3 uvIndices;
	};

	struct MTL
	{
		Vec3 ambient;
		Vec3 diffuse;
		Vec3 specular;
		float shininess;
		float transparency;
		size_t idxFaceFrom;
		int8_t illum;
		std::string name;
		std::string textureMap;
	};

	struct Mesh
	{
		std::vector<Vec3> vertices;
		std::vector<Vec2> uvs;
		std::vector<Vec3> normals;
		std::vector<unsigned int> indices;
		std::vector<MTL> materials;
		bool bSmoothShading;
	};

	std::unique_ptr<Mesh> parseOBJ(const std::string& pathTo, const std::string& name);
};

