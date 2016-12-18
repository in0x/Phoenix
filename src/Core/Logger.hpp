#pragma once

#include "PhiCoreRequired.hpp"

namespace Phoenix::Logger
{
	enum ColorCode
	{
		RED = 31,
		GREEN = 32,
		YELLOW = 33,
		LIGHT_RED = 91,
		LIGHT_GREEN = 92,
		LIGHT_YELLOW = 93
	};

	inline void Log(const std::string& msg)
	{
		std::cout << msg << "\n";
	}

	inline void Warning(const std::string& msg)
	{
		std::cout << "\033[" << ColorCode::LIGHT_YELLOW << "m" << msg << "\n";
	}

	inline void Error(const std::string& msg)
	{
		std::cout << "\033[" << ColorCode::LIGHT_RED << "m" << msg << "\n";
	}
}