#include "Logger.hpp"
#include <ctime>
#include <string>
#include <iostream> 
#include <fstream>
#include <memory>

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
			std::ofstream m_file;
			
		public:
			LogOut(bool bLogToConsole, bool bLogToFile);
			~LogOut();
			void Log(const std::string& msg);
			void Warning(const std::string& msg);
			void Error(const std::string& msg);

			bool m_bLogToConsole;
			bool m_bLogToFile;
			bool m_bUseAnsiColors; // A better solution is to have a fixed pre-/post-amble that can include things like the color code.
		};

		LogOut::LogOut(bool bLogToConsole, bool bLogToFile)
			: m_bLogToConsole(bLogToConsole)
			, m_bLogToFile(bLogToFile)
			, m_bUseAnsiColors(false)
		{
			if (m_bLogToFile)
			{
				std::string path("../tmp/Logs/");
				std::time_t t = std::time(0);
				path += std::to_string(t);
				path += "_log.txt";

				m_file.open(path);
			}
		}

		LogOut::~LogOut()
		{
			m_file.close();
		}

		void LogOut::Log(const std::string& msg)
		{
			if (m_bLogToConsole)
			{
				if (m_bUseAnsiColors)
				{
					std::cout << "\033[" << ColorCode::LIGHT_GREEN << "m";
				}
				
				std::cout << msg << "\n";
			}
			if (m_bLogToFile)
			{
				m_file << "LOG: " << msg << "\n";
			}
		}

		void LogOut::Warning(const std::string& msg)
		{
			if (m_bLogToConsole)
			{
				if (m_bUseAnsiColors)
				{
					std::cout << "\033[" << ColorCode::LIGHT_YELLOW << "m";
				}

				std::cout << msg << "\n";
			}
			if (m_bLogToFile)
			{
				m_file << "WARNING: " << msg << "\n";
			}
		}

		void LogOut::Error(const std::string& msg)
		{
			if (m_bLogToConsole)
			{
				if (m_bUseAnsiColors)
				{
					std::cout << "\033[" << ColorCode::LIGHT_RED << "m";
				}

				std::cout << msg << "\n";
			}
			if (m_bLogToFile)
			{
				m_file << "ERROR: " << msg << "\n";
			}
		}

		// I prefer managing the lifetime of the instance externally and then registering an instance that can 
		// accessed globally, so that I can have a clear order of initialization. When other globally accessible 
		// service get implemented we might want a central service directory ala ModularFeature in UE.	
		// The logging will have to be reworked at some point as well, this would be a nightmare to use with multiple threads.
		namespace
		{
			std::unique_ptr<LogOut> g_pInstance;
		}

		void init(bool bLogToConsole, bool bLogToFile)
		{
			g_pInstance = std::make_unique<LogOut>(bLogToConsole, bLogToFile);
		}

		LogOut& get()
		{
			return *g_pInstance;
		}

		void exit()
		{
			g_pInstance = nullptr;
		}

		void log(const std::string& msg)
		{
			get().Log(msg);
		}

		void warning(const std::string& msg)
		{
			get().Warning(msg);
		}

		void error(const std::string& msg)
		{
			get().Error(msg);
		}

		void setAnsiColorEnabled(bool enabled)
		{
			get().m_bUseAnsiColors = enabled;
		}
	}
}
