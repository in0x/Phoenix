#pragma once

#include <fstream>

namespace Phoenix
{
	namespace Logger
	{	
		void init(bool bLogToConsole, bool bLogToFile);
		void exit();

		void log(const std::string& msg);
		void warning(const std::string& msg);
		void error(const std::string& msg);

#define S1(x) #x
#define S2(x) S1(x)
#define __LOCATION_INFO__ "In: " __FILE__ "\nAt: " S2(__LINE__) ", " __FUNCTION__ "() " 
	}
}