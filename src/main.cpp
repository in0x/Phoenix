#include <cassert>

#include "Vec3.h"
#include "Matrix4.h"

// Check Game Coding Complete for info on WinMainLoop
int main()
{
	Phoenix::Vec3 a{ 1,0,0 };
	Phoenix::Vec3 b{ 0,1,0 };

	assert(a.dot(b) == 0);
	assert(Phoenix::Vec3( 0,1,0 ).reflect(Phoenix::Vec3(0.5, 0.5, 0)) == Phoenix::Vec3(-0.5, 0.5, 0));
	
	std::cout << Phoenix::Matrix4::unit() * Phoenix::Matrix4::unit() << std::endl;
	return 0;
}