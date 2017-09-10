#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define PLATFORM_WINDOWS
#endif

#ifndef PLATFORM_WINDOWS
static_assert(false, __FILE__ " may only be included on a Windows Platform");
#endif

#include <Windows.h>

namespace Phoenix
{
	namespace Platform
	{
		inline bool enableConsoleAnsiColors()
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

			return true;
#else
			return false;
#endif
	}
}
}
