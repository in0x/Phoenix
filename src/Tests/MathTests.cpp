#include "MathTests.hpp"
#include <cassert>
#include <iostream>
#include <Math/PhiMath.hpp>
#include <Math/MathStreamOverloads.hpp>
#include <Core/Clock.hpp>

namespace Phoenix { namespace Tests
{
	void runMathTests()
	{
		vec3Tests();
		matrix4Tests();
		planeTests();
	}

	void vec3Tests()
	{
		Vec3 a{ 1,0,0 };
		Vec3 b{ 0,1,0 };

		assert(a(0) == 1.f);

		assert(a.dot(b) == 0);
		auto refl = Vec3(0, 1, 0).reflect(Vec3(0.5, 0.5, 0));
		assert(Vec3(0, 1, 0).reflect(Vec3(0.5, 0.5, 0)) == Vec3(-0.5, 0.5, 0));
	}

	void matrix4Tests()
	{
		assert(Matrix4::identity().transpose() == Matrix4::identity());
		assert(Matrix4::identity().determinant() == 1.f);
		assert(Matrix4(4, 5, 3, 7,
			8, 5, 6, 2,
			21, 4, 3, 21,
			2, 1, 12, 48).determinant() == 20172);
		assert(Matrix4::identity().inverse() == Matrix4::identity());

		assert(Matrix4::identity() * Matrix4::identity() == Matrix4::identity());

		Matrix4 matA{ 1,4,2,3,
			2,5,2,1,
			2,5,8,1,
			9,1,4,7 };

		Matrix4 matB{ 0,1,2,0,
			5,7,0,9,
			3,4,2,6,
			1,9,7,6 };

		assert((matA * matB) == Matrix4(29, 64, 27, 66,
			32, 54, 15, 63,
			50, 78, 27, 99,
			24, 95, 75, 75));

		Matrix4 mult{ 1, 2, 3, 4,
			1, 1, 1, 1,
			2, 1, 3, 4,
			1, 1, 1, 1 };

		assert((Vec4(1, 1, 1, 1) *= mult) == Vec4(10, 4, 10, 4));
		assert((Vec3(1, 1, 1) *= mult) == Vec3(6, 3, 6));
	}

	void planeTests()
	{
		Plane p1{ { 0,0,0 }, { 0,0,1 }, { 1,0,0 } };

		assert(p1.getSideOn({ 0, 0, 0 }) == Plane::ESide::ON);
		assert(p1.getSideOn({ 0, 1, 0 }) == Plane::ESide::FRONT);
		assert(p1.getSideOn({ 0,-1, 0 }) == Plane::ESide::BACK);

		assert(p1.distance({ 0, 0, 0 }) == 0);
		assert(std::abs(p1.distance({ 0, 10, 0 })) == 10);

		assert(p1.reflect({ 1, 1, 0 }) == Vec3(1, -1, 0));

		Plane p2{ { 0,0,0 }, { 1,0,0 }, { 0,1,0 } };

		assert(p2.getSideOn({ 0, 0, 0 }) == Plane::ESide::ON);
		assert(p2.getSideOn({ 0, 0, 1 }) == Plane::ESide::FRONT);
		assert(p2.getSideOn({ 0, 0,-1 }) == Plane::ESide::BACK);

		assert(p2.distance({ 0, 0, 0 }) == 0);
		assert(std::abs(p2.distance({ 0, 10, 0 })) == 0);
		assert(std::abs(p2.distance({ 0, 0, 10 })) == 10);

		assert(p2.intersect(Ray{ {0,1,-1}, {0,0,1} }).first == true);

		assert(p1.intersect(p2).first == true);
		assert(p2.intersect(p1).first == true);
	}
}
}