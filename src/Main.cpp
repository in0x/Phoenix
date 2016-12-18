#include <cassert>

#include "Core\Engine.hpp"

#include "Tests/MathTests.hpp"
#include "Core/ATK.hpp"

using namespace Phoenix;

// Check Game Coding Complete for info on WinMainLoop
int main(int arc, char** argv)
{
	Tests::MathTests::RunMathTests();
	auto scene = ATK::parseOBJ("Fox/", "RedFox.obj");

	Engine theEngine;
	theEngine.init();
	theEngine.update();
	theEngine.exit();

	return 0;
}