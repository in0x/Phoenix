#include <cassert>

#include "Tests/MathTests.hpp"
#include "Core/ATK.hpp"

#include "Windows.h"
#include <direct.h>

using namespace Phoenix;

// Check Game Coding Complete for info on WinMainLoop
int main()
{
	//Tests::MathTests::RunMathTests();

	char buffer[MAX_PATH];
	char *answer = _getcwd(buffer, sizeof(buffer));
	std::string s_cwd;

	if (answer)
	{
		s_cwd = answer;
	}
	std::cout << "CWD: " << s_cwd << "\n";

	auto pScene = ATK::parseOBJ("Fox/RedFox.obj");

	return 0;
}