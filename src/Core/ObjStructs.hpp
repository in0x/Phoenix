#pragma once

#include "Math/PhiMath.hpp"
#include <vector>

namespace Phoenix
{
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
}

