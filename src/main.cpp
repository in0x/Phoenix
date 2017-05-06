#include <cassert>

#include "Tests/MathTests.hpp"
#include "Core/obj.hpp"

char* getCMDOption(char** start, char** end, const std::string& option)
{
	char** iter = std::find(start, end, option);

	if (iter != end && ++iter != end)
	{
		return *iter;
	}

	return nullptr;
}

int main(int argc, char** argv)
{
	using namespace Phoenix;

	Logger::LogOut::get().logToConsole = true;
	Logger::LogOut::get().logToFile = false;

	Tests::RunMathTests();

	std::unique_ptr<Mesh> fox = parseOBJ("Fox/", "RedFox.obj");
	assert(fox != nullptr);

	
	int windowWidth = 1280;
	int windowHeight = 720;

	return 0;
}