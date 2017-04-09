#include <cassert>

#include "Core\Engine.hpp"

#include "Tests/MathTests.hpp"
#include "Core/ATK.hpp"

#include "gl3w/gl3w.h"
#include "GLFW/glfw3.h"

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

	std::unique_ptr<ATK::Mesh> fox = ATK::parseOBJ("Fox/", "RedFox.obj");
	assert(fox != nullptr);

	return 0;
}