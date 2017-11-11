#pragma once

#include <memory>
#include <vector>

#include <Core/Mesh.hpp>
#include <Math/Vec2.hpp>
#include <Math/Vec3.hpp>

namespace Phoenix
{
	struct MTL
	{
		std::string name;
		std::string textureMap;
		Vec3 ambient;
		Vec3 diffuse;
		Vec3 specular;
		float shininess;
		float transparency;
		size_t idxFaceFrom;
		int8_t illum;		
	};
	
	struct OBJMesh
	{
		std::vector<Vec3> vertices;
		std::vector<Vec2> uvs;
		std::vector<Vec3> normals;
		std::vector<uint32_t> indices;
	};

	struct OBJImport
	{
		OBJMesh mesh;
		std::vector<MTL> materials;
		bool bSmoothShading;
	};

	OBJImport loadObj(const std::string& pathTo, const std::string& name);
};

