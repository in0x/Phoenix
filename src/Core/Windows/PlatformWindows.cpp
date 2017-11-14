#include "PlatformWindows.hpp"
#include "PhiWindowsInclude.hpp"
#include <algorithm>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

namespace Phoenix
{
	//SetProcessDPIAware();

	namespace Platform
	{
		// Credit console color: https://gist.github.com/mlocati/21a9233ac83f7d3d7837535bc109b3b7
		typedef NTSTATUS(WINAPI*RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

		bool consoleHasColorSupport()
		{
			const DWORD MINV_MAJOR = 10, MINV_MINOR = 0, MINV_BUILD = 10586;
			HMODULE hMod = GetModuleHandle(TEXT("ntdll.dll"));

			if (!hMod)
			{
				return false;
			}

			RtlGetVersionPtr rlGetVersion = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");

			if (rlGetVersion == NULL)
			{
				return false;
			}

			RTL_OSVERSIONINFOW rovi = { 0 };
			rovi.dwOSVersionInfoSize = sizeof(rovi);

			if (rlGetVersion(&rovi) != 0)
			{
				return false;
			}

			if (rovi.dwMajorVersion > MINV_MAJOR || (rovi.dwMajorVersion == MINV_MAJOR && (rovi.dwMinorVersion > MINV_MINOR || (rovi.dwMinorVersion == MINV_MINOR && rovi.dwBuildNumber >= MINV_BUILD))))
			{
				return true;
			}		

			return false;
		}

		bool enableConsoleColor(bool enabled)
		{
			if (!consoleHasColorSupport())
			{
				return false;
			}

			HANDLE hStdOut;
			hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

			if (INVALID_HANDLE_VALUE == hStdOut)
			{
				return false;
			}

			DWORD mode;
			if (!GetConsoleMode(hStdOut, &mode))
			{
				return false;
			}

			if (((mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) ? 1 : 0) == (enabled ? 1 : 0))
			{
				return true;
			}
			else
			{
				if (enabled)
				{
					mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				}
				else
				{
					mode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				}
				if (SetConsoleMode(hStdOut, mode))
				{
					return true;
				}
			}

			return false;
		}

		char* getCMDOption(char** start, char** end, const char* option)
		{
			char** iter = std::find(start, end, option);

			if (iter != end && ++iter != end)
			{
				return *iter;
			}

			return nullptr;
		}
	}
}