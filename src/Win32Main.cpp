#include <cassert>

#include "Core\Engine.hpp"

#include "Tests/MathTests.hpp"
#include "Core/ATK.hpp"
#include <Windows.h>
#include "Core\Win32Window.hpp"

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

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Phoenix::Logger::LogOut::get().logToConsole = true;
	Phoenix::Logger::LogOut::get().logToFile = true;

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