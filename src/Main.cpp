#include <cassert>

#include "Core\Engine.hpp"

#include "Tests/MathTests.hpp"
#include "Core/ATK.hpp"

using namespace Phoenix;

/*
	TODO:
		Create a platform layer for opening a window. We want to be able to attach the window
		to either a DirectX or an OpenGL context. To reduce complexity we will only support
		Windows for now. Being able to switch between rendering apis is more important for me.
*/

//int main(int arc, char** argv)
//{
//	Tests::MathTests::RunMathTests();
//	auto scene = ATK::parseOBJ("Fox/", "RedFox.obj");
//	return 0;
//}

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
	WindowConfig config = { 1920, 1080,
							0,0,
							std::wstring(L"Phoenix"),
							false };

	Win32Window window(config);

	if (!window.isOpen())
	{
		Logger::Error("Failed to initialize Win32Window");
		return -1;
	}

	Logger::LogOut::get().logToConsole = true;
	Logger::LogOut::get().logToFile = true;
	
	MSG msg;
	msg.message = ~WM_QUIT;

	window.isOpen();

	while (msg.message != WM_QUIT)
	{
		DWORD result = MsgWaitForMultipleObjectsEx(0, nullptr, 1.f / 60.f, QS_ALLEVENTS, 0);
	
		if (result != WAIT_TIMEOUT)
		{
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	return 0;
}