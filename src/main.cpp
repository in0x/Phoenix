#include "Vec3.h"
#include <cassert>

int main()
{
	using namespace Phoenix;
	
	Vec3 a{ 1,0,0 };
	Vec3 b{ 0,1,0 };

	assert(a.dot(b) == 0);
	assert(Vec3( 0,1,0 ).reflect(Vec3(0.5, 0.5, 0)) == Vec3(-0.5, 0.5, 0));
	
	return 0;
}