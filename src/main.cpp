#include "Vec3.h"
#include "VecN.h"
#include <cassert>

int main()
{
	using namespace Phoenix;
	
	Vec3 a{ 1,0,0 };
	Vec3 b{ 0,1,0 };

	assert(a.dot(b) == 0);
	assert(Vec3( 0,1,0 ).reflect(Vec3(0.5, 0.5, 0)) == Vec3(-0.5, 0.5, 0));
	
	Vec<3> i{ 1.f, 2.f, 3.f };
	Vec<3> j{ 1.f, 2.f, 3.f };

	i + j;

	//Vec3f{ 1.2f };
	//Vec3f{ 1.1f, 1.2f };

	return 0;
}