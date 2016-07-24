#include <cassert>

#include "Vec3.h"
#include "Matrix4.h"

// Check Game Coding Complete for info on WinMainLoop
int main()
{
	using namespace Phoenix;

	Vec3 a{ 1,0,0 };
	Vec3 b{ 0,1,0 };

	assert(a.dot(b) == 0);
	assert(Vec3( 0,1,0 ).reflect(Vec3(0.5, 0.5, 0)) == Vec3(-0.5, 0.5, 0));
	assert(Matrix4::identity().transpose() == Matrix4::identity());
	assert(Matrix4::identity().determinant() == 1.f);
	assert(Matrix4( 4, 5, 3, 7,
					8, 5, 6, 2,
					21, 4, 3, 21, 
					2, 1, 12, 48).determinant() == 20172);
	assert(Matrix4::identity().inverse() == Matrix4::identity());

	return 0;
}