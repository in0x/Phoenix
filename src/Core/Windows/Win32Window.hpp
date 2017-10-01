#pragma once
#include <memory>
#include <string>
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
		bool autoShow;
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
		virtual ~Win32Window();

		static LRESULT CALLBACK OnEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

		void processMessages();
		void minimize();
		void maximize();
		void setVisible(bool visible);
		bool isFullscreen() const;
		bool isOpen() const;
		void setFullscreen(bool fullscreen);
		Size getDimensions() const;
		void resize(unsigned int width, unsigned int height);
		HWND getNativeHandle();

	private:
		static unsigned int s_windowCount;

		struct Pimpl;
		std::unique_ptr<Pimpl> self;

		void processEvent(UINT message, WPARAM wParam, LPARAM lParam);
		void OnResize(int width, int height);
	};
}