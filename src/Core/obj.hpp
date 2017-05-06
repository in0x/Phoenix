#pragma once

/*
	For loading Asset files. This will ony support .obj
	at first since most of the models I own are in that format.
*/

#include "PhiCoreRequired.hpp"
#include "Logger.hpp"
#include <fstream>
#include "Math/PhiMath.hpp"

namespace Phoenix
{
	struct Face
	{
		Vec3 vertexIndices;
		Vec3 normalIndices;
		Vec3 uvIndices;
	};

	struct Material
	{
		Vec3 ambient;
		Vec3 diffuse;
		Vec3 specular;
		float shininess;
		float transperency;
		int idxFaceFrom;
		int8_t illum;
		std::string name;
		std::string textureMap;
	};

	struct Mesh
	{
		std::vector<Vec3> vertices;
		std::vector<Vec2> uvs;
		std::vector<Vec3> normals;
		std::vector<Face> faces;
		std::vector<Material> materials;
		bool bSmoothShading;
	};

	std::unique_ptr<Mesh> parseOBJ(const std::string& pathTo, const std::string& name);
};

