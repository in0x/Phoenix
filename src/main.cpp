#include "Vec3.h"

int main()
{
	Phoenix::Vec3 vec{ 1,2,3 };
	Phoenix::Vec3 vec2{ -1,0,0 };

	std::cout << vec + vec2 << std::endl;

	return 0;
}