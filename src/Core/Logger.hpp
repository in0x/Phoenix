#pragma once

namespace Phoenix
{
	namespace Logger
	{	
		void init(bool bLogToConsole, bool bLogToFile, size_t maxMsgLength);
		void setAnsiColorEnabled(bool enabled);
		void exit();

		void log(const char* msg);
		void warning(const char* msg);
		void error(const char* msg);

		void logf(const char* format, ...);
		void warningf(const char* format, ...);
		void errorf(const char* format, ...);

#define S1(x) #x
#define S2(x) S1(x)
#define __LOCATION_INFO__ "In: " __FILE__ "\nAt: " S2(__LINE__) ", " __FUNCTION__ "() " 
	}
}