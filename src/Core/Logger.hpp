#pragma once

#include <fstream>

namespace Phoenix
{
	namespace Logger
	{	
		void init(bool bLogToConsole, bool bLogToFile);
		void setAnsiColorEnabled(bool enabled);
		void exit();

		void log(const std::string& msg);
		void warning(const std::string& msg);
		void error(const std::string& msg);

		//template <class... Args>
		//void log(const char* format, Args... args)
		//{
		//	size_t bufferSize = snprintf(nullptr, 0, format, args...);
		//	std::string buffer(bufferSize, ' ');
		//	snprintf(buffer.data(), bufferSize + 1, format, args...);
		//}

#define S1(x) #x
#define S2(x) S1(x)
#define __LOCATION_INFO__ "In: " __FILE__ "\nAt: " S2(__LINE__) ", " __FUNCTION__ "() " 
	}
}