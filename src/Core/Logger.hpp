#pragma once

#include "PhiCoreRequired.hpp"
#include <fstream>

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
				m_file << "LOG: " << msg;
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
				m_file << "WARNING: " << msg;
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
				m_file << "ERROR: " << msg;
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

}