#pragma once
#include "PhiCoreRequired.hpp"
#include <Windows.h>
#include "IWindow.hpp"

namespace Phoenix
{
	class Win32Window : public IWindow
	{
	public:

		Win32Window(const WindowConfig& config);
		virtual ~Win32Window();

		static LRESULT CALLBACK OnEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

		bool init() override;
		void minimize() override;
		void maximize() override;
		void show() override;
		void hide() override;
		bool isFullscreen() const override;
		bool isOpen() const override;
		void setFullscreen(bool fullscreen) override;
		Size getDimensions() const override;
		void resize(unsigned int width, unsigned int height) override;

	private:
		WindowConfig m_config;
		HWND m_window;

		void processEvent(UINT message, WPARAM wParam, LPARAM lParam);
		void OnResize(int width, int height);
	};
}