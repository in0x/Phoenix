#pragma once

namespace Phoenix
{
	namespace Platform
	{
		bool consoleHasColorSupport();

		bool enableConsoleColor(bool enabled);

		char* getCMDOption(char** start, char** end, const char* option);
	}
}
