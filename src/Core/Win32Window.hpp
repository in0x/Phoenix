#pragma once
#include "PhiCoreRequired.hpp"
#include <Windows.h>

namespace Phoenix
{
	struct WindowConfig
	{
		unsigned int width;
		unsigned int height;
		unsigned int left;
		unsigned int top;
		std::wstring windowName;
		bool fullscreen;
		bool closed;
	};

	class Win32Window 
	{
	public:

		struct Size
		{
			unsigned int width;
			unsigned int height;
		};

		// Can fail. Check isOpen() to see if window creation was successful.
		Win32Window(const WindowConfig& config);
		Win32Window(Win32Window&) = delete;
		Win32Window(Win32Window&&) = delete;
		virtual ~Win32Window();

		static LRESULT CALLBACK OnEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

		void processMessages();
		void minimize();
		void maximize() ;
		void setVisible(bool visible) ;
		bool isFullscreen() const ;
		bool isOpen() const ;
		void setFullscreen(bool fullscreen) ;
		Size getDimensions() const ;
		void resize(unsigned int width, unsigned int height) ;

	private:
		static unsigned int s_windowCount;

		unsigned int left;
		unsigned int top;
		unsigned int width;
		unsigned int height;
		bool fullscreen;
		HWND window;
		MSG msg;
		std::wstring name;

		void processEvent(UINT message, WPARAM wParam, LPARAM lParam);
		void OnResize(int width, int height);
	};
}