#include <cassert>

#include "phiMath.hpp"

namespace Phoenix::Tests
{
	/*
		Write some test to check speed of current matrix copying speed
		vs move-based implementation
	*/

	static void BasicMathTest()
	{
		using namespace Phoenix::Math;

		Vec3 a{ 1,0,0 };
		Vec3 b{ 0,1,0 };

		assert(a.dot(b) == 0);
		assert(Vec3(0, 1, 0).reflect(Vec3(0.5, 0.5, 0)) == Vec3(-0.5, 0.5, 0));
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

		//std::cout << lookAtRH(Vec3{ 0,0,2 }, Vec3{ 0,0,0 }, Vec3{ 0,1,0 }) << std::endl;
		std::cout << projectionRH(90.f, 1920.f / 1080.f, 0.5f, 20.f, ProjectionType::PERSPECTIVE) << std::endl;

		float det = Matrix3(3, 1, 2,
							4, -1, 2,
							9, 2, 3).determinant();

		assert(det == 19.f);
	}
}