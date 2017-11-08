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

	struct OBJImport
	{
		std::unique_ptr<Mesh> mesh;
		std::vector<MTL> materials;
		bool bSmoothShading;
	};

	OBJImport loadObj(const std::string& pathTo, const std::string& name);
};

