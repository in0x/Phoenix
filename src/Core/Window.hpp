#pragma once

#include <Windows.h>
#include "PhiCoreRequired.hpp"
#include "Logger.hpp"

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

	class Window
	{
	public:
		struct Size
		{
			unsigned int width;
			unsigned int height;
		};


		Window() {}
		Window(Window&) = delete;
		Window(Window&&) = delete;

		virtual bool init() = 0;
		virtual void minimize() = 0;
		virtual void maximize() = 0;
		virtual void show() = 0;
		virtual void hide() = 0;
		virtual bool isFullscreen() = 0;
		virtual bool isOpen() = 0;
		virtual void setFullscreen(bool fullscreen) = 0;
		virtual Size getDimensions() = 0;
		virtual void resize(unsigned int width, unsigned int height) = 0;
	};

	class Win32Window : Window
	{
	private:
		WindowConfig m_config;
		HWND m_window;
	public:

		Win32Window(const WindowConfig& config)
			: m_config(config) {}

		virtual ~Win32Window()
		{
			if (m_config.fullscreen)
				ChangeDisplaySettings(nullptr, 0);

			DestroyWindow(m_window);
			UnregisterClass(m_config.windowName.c_str(), GetModuleHandle(nullptr));
		};

		bool init() override
		{
			WNDCLASS windowClass;

			windowClass.style = 0;
			windowClass.lpfnWndProc = &Win32Window::OnEvent;
			windowClass.cbClsExtra = 0;
			windowClass.cbWndExtra = 0;
			windowClass.hInstance = GetModuleHandle(nullptr);
			windowClass.hIcon = nullptr;
			windowClass.hCursor = 0;
			windowClass.hbrBackground = 0;
			windowClass.lpszMenuName = nullptr;
			windowClass.lpszClassName = m_config.windowName.c_str();

			RegisterClass(&windowClass);

			DWORD exWindStyle = 0;
			DWORD windStyle = 0;

			RECT rect = { m_config.left,
						 m_config.top,
						 m_config.left + m_config.width,
						 m_config.top + m_config.height };

			if (m_config.fullscreen)
			{
				DEVMODE displayConfig = {};
				displayConfig.dmSize = sizeof(displayConfig);
				displayConfig.dmPelsWidth = m_config.width;
				displayConfig.dmPelsHeight = m_config.height;
				displayConfig.dmBitsPerPel = 32;
				displayConfig.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

				if (ChangeDisplaySettings(&displayConfig, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
				{
					Logger::Error("Failed to fullscreen the window.");
					return false; // TODO: cleanup?
				}

				exWindStyle = WS_EX_APPWINDOW;
				windStyle = WS_POPUP;
			}
			else
			{
				exWindStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
				windStyle = WS_OVERLAPPEDWINDOW;
			}

			// Reconfigures window rect to factor in size of border etc.
			AdjustWindowRectEx(&rect, windStyle, FALSE, exWindStyle);

			m_window = CreateWindowEx(exWindStyle,
				m_config.windowName.c_str(),
				m_config.windowName.c_str(),
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN | windStyle,
				m_config.left, m_config.top,
				m_config.width, m_config.height,
				nullptr, // Parent window
				nullptr,
				GetModuleHandle(nullptr),
				this);

			if (!m_window)
			{
				Logger::Error("Failed to create window.");
				return false; // TODO: cleanup?
			}

			ShowWindow(m_window, SW_SHOW);
		}

		/*
			WPARAM -> Word parameter, carries "words" i.e. handle, integers
			LAPARM -> Long paramter -> carries pointers
		*/
		static LRESULT CALLBACK OnEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
		{
			if (message == WM_NCCREATE)
			{
				CREATESTRUCT* createParams = reinterpret_cast<CREATESTRUCT*>(lParam);
				SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<long>(
					createParams->lpCreateParams));
			}

			Win32Window* pWindow = reinterpret_cast<Win32Window*>
				(GetWindowLongPtr(handle, GWLP_USERDATA));

			if (pWindow)
				pWindow->processEvent(message, wParam, lParam);

			return DefWindowProc(handle, message, wParam, lParam);
		}


		void minimize() override {}
		void maximize() override {}
		void show() override {}
		void hide() override {}
		bool isFullscreen() override { return m_config.fullscreen; }
		bool isOpen() override { return !m_config.closed; }
		void setFullscreen(bool fullscreen) override {}
		Size getDimensions() override { return{ m_config.width, m_config.height }; }
		void resize(unsigned int width, unsigned int height) override {}

	private:
		void processEvent(UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_CLOSE:
				PostQuitMessage(0);
				m_config.closed = true;
				break;

			case WM_SIZE:
				OnResize(LOWORD(lParam), HIWORD(lParam));
				break;

			case WM_KEYDOWN:
				break;
			
			case WM_KEYUP:
				break;
			}
		}

		void OnResize(int width, int height)
		{
		
		}

	};
}