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

	return 0;
}