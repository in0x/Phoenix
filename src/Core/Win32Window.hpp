#pragma once
#include "PhiCoreRequired.hpp"
#include <Windows.h>
#include "IWindow.hpp"

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

	class Win32Window : public IWindow
	{
	public:

		// Can fail. Check isOpen() to see if window creation was successful.
		Win32Window(const WindowConfig& config);
		virtual ~Win32Window();

		static LRESULT CALLBACK OnEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

		void minimize() override;
		void maximize() override;
		void setVisible(bool visible) override;
		bool isFullscreen() const override;
		bool isOpen() const override;
		void setFullscreen(bool fullscreen) override;
		Size getDimensions() const override;
		void resize(unsigned int width, unsigned int height) override;

	private:
		static unsigned int s_windowCount;

		struct Pimpl;
		std::unique_ptr<Pimpl> self;
		
		void processEvent(UINT message, WPARAM wParam, LPARAM lParam);
		void OnResize(int width, int height);
	};
}