#pragma once

#include <Windows.h>

inline void initPlatform()
{

#ifdef ENABLE_VIRTUAL_TERMINAL_PROCESSING
	// NOTE(Phil): Enables ansi color codes being interpreted by the console.
	// I need a place to put this so its only called when we are on Windows.
{
	// Set output mode to handle virtual terminal sequences
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
}
#endif

	SetProcessDPIAware();
}