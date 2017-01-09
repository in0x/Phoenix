#pragma once

#include "PhiCoreRequired.hpp"
#include <fstream>

namespace Phoenix
{
	namespace Logger
	{
		namespace
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
		}

		class LogOut
		{
		public:
			bool logToConsole;
			bool logToFile;

			static LogOut& get()
			{
				static LogOut sSingleton;
				return sSingleton;
			}

			void Log(const std::string& msg)
			{
				if (logToConsole)
				{
					std::cout << msg << "\n";
				}
				if (logToFile)
				{
					m_file << "LOG: " << msg << "\n";
				}
			}

			void Warning(const std::string& msg)
			{
				if (logToConsole)
				{
					std::cout << "\033[" << ColorCode::LIGHT_YELLOW << "m" << msg << "\n";
				}
				if (logToFile)
				{
					m_file << "WARNING: " << msg << "\n";
				}
			}

			void Error(const std::string& msg)
			{
				if (logToConsole)
				{
					std::cout << "\033[" << ColorCode::LIGHT_RED << "m" << msg << "\n";
				}
				if (logToFile)
				{
					m_file << "ERROR: " << msg << "\n";
				}
			}

		private:
			std::ofstream m_file;

			LogOut()
			{
				std::string path("../tmp/Logs/");
				std::time_t t = std::time(0);
				path += std::to_string(t);
				path += "_log.txt";

				m_file.open(path);
			}
		};

#define S1(x) #x
#define S2(x) S1(x)
#define __LOCATION_INFO__ "In: " __FILE__ "\nAt: " S2(__LINE__) ", " __FUNCTION__ "\n" 

		inline void Log(const std::string& msg)
		{
			LogOut::get().Log(msg);
		}

		inline void Warning(const std::string& msg)
		{
			LogOut::get().Warning(msg);
		}

		inline void Error(const std::string& msg)
		{
			LogOut::get().Error(msg);
		}

		//#define LOG(x) Logger::LogOut::get().Log("LOG " __LOCATION_INFO__ "In: " __FILE__ "\nAt: " S2(__LINE__) ", " __FUNCTION__ "\n" x) 
	}
}