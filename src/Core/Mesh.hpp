#pragma once

#include <vector>
#include <Math/Vec2.hpp>
#include <Math/Vec3.hpp>

namespace Phoenix
{
	struct Mesh
	{
		std::vector<Vec3> vertices;
		std::vector<Vec2> uvs;
		std::vector<Vec3> normals;
		std::vector<uint32_t> indices;
	};
}