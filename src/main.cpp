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
	
	std::cout << Matrix4::identity() * Matrix4::identity() << std::endl;
	std::cout << Matrix4{ 1, 2, 3, 4,
									5, 6, 7, 8,
									1, 2, 3, 4,
									5, 6, 7, 8}.transpose() << std::endl;

	return 0;
}