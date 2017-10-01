#include "Logger.hpp"
#include <ctime>
#include <string>
#include <iostream> 
#include <fstream>
#include <memory>
#include <cstdarg>

namespace Phoenix
{
	namespace Logger
	{
		namespace
		{
			enum EColorCode
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
			LogOut(bool bLogToConsole, bool bLogToFile, size_t maxMsgLength);
			~LogOut();
			void Log(const char* msg);
			void Warning(const char* msg);
			void Error(const char* msg);

			char* m_msgBuffer;
			size_t m_msgBufferSize; 

			bool m_bLogToConsole;
			bool m_bLogToFile;
			bool m_bUseAnsiColors; // A better solution is to have a fixed pre-/post-amble that can include things like the color code.
		};

		LogOut::LogOut(bool bLogToConsole, bool bLogToFile, size_t maxMsgLength)
			: m_bLogToConsole(bLogToConsole)
			, m_bLogToFile(bLogToFile)
			, m_bUseAnsiColors(false)
			, m_msgBuffer(nullptr)
			, m_msgBufferSize(maxMsgLength)
		{
			if (m_bLogToFile)
			{
				std::string path("../tmp/Logs/");
				std::time_t t = std::time(0);
				path += std::to_string(t);
				path += "_log.txt";

				m_file.open(path);
			}

			m_msgBuffer = new char[maxMsgLength + 1];
		}

		LogOut::~LogOut()
		{
			delete[] m_msgBuffer;
			m_file.close();
		}

		void LogOut::Log(const char* msg)
		{
			if (m_bLogToConsole)
			{
				if (m_bUseAnsiColors)
				{
					std::cout << "\033[" << EColorCode::LIGHT_GREEN << "m";
				}

				std::cout << msg << "\n";
			}
			if (m_bLogToFile)
			{
				m_file << "LOG: " << msg << "\n";
			}
		}

		void LogOut::Warning(const char* msg)
		{
			if (m_bLogToConsole)
			{
				if (m_bUseAnsiColors)
				{
					std::cout << "\033[" << EColorCode::LIGHT_YELLOW << "m";
				}

				std::cout << msg << "\n";
			}
			if (m_bLogToFile)
			{
				m_file << "WARNING: " << msg << "\n";
			}
		}

		void LogOut::Error(const char* msg)
		{
			if (m_bLogToConsole)
			{
				if (m_bUseAnsiColors)
				{
					std::cout << "\033[" << EColorCode::LIGHT_RED << "m";
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
		// service g_pInstance->implemented we might want a central service directory ala ModularFeature in UE.	
		// The logging will have to be reworked at some point as well, this would be a nightmare to use with multiple threads.
		namespace
		{
			std::unique_ptr<LogOut> g_pInstance;
		}

		void init(bool bLogToConsole, bool bLogToFile, size_t maxMsgLength)
		{
			g_pInstance = std::make_unique<LogOut>(bLogToConsole, bLogToFile, maxMsgLength);
		}

		void exit()
		{
			g_pInstance = nullptr;
		}

		void log(const char* msg)
		{
			g_pInstance->Log(msg);
		}

		void warning(const char* msg)
		{
			g_pInstance->Warning(msg);
		}

		void error(const char* msg)
		{
			g_pInstance->Error(msg);
		}

		void setAnsiColorEnabled(bool enabled)
		{
			g_pInstance->m_bUseAnsiColors = enabled;
		}

		// NOTE(Phil): Consider using STB sprintf if this hits really hard.
		void formatString(const char* format, va_list args)
		{
			size_t bufferSize = vsnprintf(nullptr, 0, format, args);
			bufferSize = bufferSize < g_pInstance->m_msgBufferSize ? bufferSize : g_pInstance->m_msgBufferSize;
			bufferSize += 1;
			vsnprintf(g_pInstance->m_msgBuffer, bufferSize, format, args);
		}

		void clearMsgBuffer()
		{
			g_pInstance->m_msgBuffer[0] = 0;
		}

		void logf(const char* format, ...)
		{
			va_list args;
			va_start(args, format);
			formatString(format, args);
			va_end(args);

			log(g_pInstance->m_msgBuffer);
			clearMsgBuffer();
		}
		
		void warningf(const char* format, ...)
		{
			va_list args;
			va_start(args, format);
			formatString(format, args);
			va_end(args);

			warning(g_pInstance->m_msgBuffer);
			clearMsgBuffer();
		}
		
		void errorf(const char* format, ...)
		{
			va_list args;
			va_start(args, format);
			formatString(format, args);
			va_end(args);

			error(g_pInstance->m_msgBuffer);
			clearMsgBuffer();
		}
	}
}
