#include <cassert>

#include "Core\Engine.hpp"

#include "Tests/MathTests.hpp"
#include "Core/ATK.hpp"

/*
	Likely structure:
		Window -> Interface class with game
		Win32Window -> Takes care of Windows specific stuff
			and is created on start up

		Engine -> actual engine class, gets a window passed to it
			on creation
*/

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
	Phoenix::Logger::LogOut::get().logToConsole = true;
	Phoenix::Logger::LogOut::get().logToFile = true;

	return 0;
}