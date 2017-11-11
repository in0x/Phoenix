#pragma once
#include "PhiWindowsInclude.hpp"
#include <algorithm>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

namespace Phoenix
{
	namespace Platform
	{
		// Credit console color: https://gist.github.com/mlocati/21a9233ac83f7d3d7837535bc109b3b7
		typedef NTSTATUS(WINAPI*RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

		BOOL consoleHasColorSupport() 
		{
			static BOOL result = 2;
			
			if (result == 2) 
			{
				const DWORD MINV_MAJOR = 10, MINV_MINOR = 0, MINV_BUILD = 10586;
				result = FALSE;
				HMODULE hMod = GetModuleHandle(TEXT("ntdll.dll"));
				
				if (hMod) 
				{
					RtlGetVersionPtr rlGetVersion = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");

					if (rlGetVersion != NULL) 
					{
						RTL_OSVERSIONINFOW rovi = { 0 };
						rovi.dwOSVersionInfoSize = sizeof(rovi);

						if (rlGetVersion(&rovi) == 0) 
						{
							if (
								rovi.dwMajorVersion > MINV_MAJOR
								||
								(
									rovi.dwMajorVersion == MINV_MAJOR
									&&
									(
										rovi.dwMinorVersion > MINV_MINOR
										||
										(
											rovi.dwMinorVersion == MINV_MINOR
											&& rovi.dwBuildNumber >= MINV_BUILD
											)
										)
									)
								) {
								result = TRUE;
							}
						}
					}
				}
			}

			return result;
		}

		bool enableConsoleColor(BOOL enabled)
		{
			BOOL result = FALSE;

			if (consoleHasColorSupport())
			{
				HANDLE hStdOut;
				hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

				if (INVALID_HANDLE_VALUE != hStdOut)
				{
					DWORD mode;
					if (GetConsoleMode(hStdOut, &mode))
					{
						if (((mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) ? 1 : 0) == (enabled ? 1 : 0))
						{
							result = TRUE;
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
							if (SetConsoleMode(hStdOut, mode)) {
								result = TRUE;
							}
						}
					}
				}
			}

			return result == TRUE;
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
