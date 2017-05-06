#include <cassert>
#include <Windows.h>
#include "Core/obj.hpp"
#include "Tests/MathTests.hpp"
#include "Core\Win32Window.hpp"

char* getCMDOption(char** start, char** end, const std::string& option)
{
	char** iter = std::find(start, end, option);

	if (iter != end && ++iter != end)
	{
		return *iter;
	}

	return nullptr;
}

/*
Likely structure:
Window -> Interface class with game
Win32Window -> Takes care of Windows specific stuff
and is created on start up
Engine -> actual engine class, gets a window passed to it
on creation
*/

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	using namespace Phoenix;

	Logger::LogOut::get().logToConsole = true;
	Logger::LogOut::get().logToFile = false;

	Tests::RunMathTests();

	std::unique_ptr<Mesh> fox = parseOBJ("Fox/", "RedFox.obj");
	assert(fox != nullptr);
	
	Phoenix::WindowConfig config = { 1600, 900,
		0,0,
		std::wstring(L"Phoenix"),
		false };

	Phoenix::Win32Window window(config);

	if (!window.isOpen())
	{
		Phoenix::Logger::Error("Failed to initialize Win32Window");
		return -1;
	}

	while (window.isOpen())
	{
		window.processMessages();
	}

	return 0;
}


