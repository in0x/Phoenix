#include "Win32Window.hpp"

namespace Phoenix
{
	Win32Window::Win32Window(const WindowConfig& config)
		: m_config(config)
	{}

	Win32Window::~Win32Window()
	{
		if (m_config.fullscreen)
			ChangeDisplaySettings(nullptr, 0);

		DestroyWindow(m_window);
		UnregisterClass(m_config.windowName.c_str(), GetModuleHandle(nullptr));
	};

	bool Win32Window::init()
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
		return true;
	}

	/*
	WPARAM -> Word parameter, carries "words" i.e. handle, integers
	LAPARM -> Long paramter -> carries pointers
	*/
	LRESULT CALLBACK Win32Window::OnEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_NCCREATE)
		{
			auto pCreateParams = reinterpret_cast<CREATESTRUCT*>(lParam);
			SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<uintptr_t>(
				/* On 64-bit we need to cast to long long, otherwise we'll get a 32-bit
				pointer which will truncate the upper 32 bits.

				TODO: Does this work on 32-bit? */
				pCreateParams->lpCreateParams));
		}

		auto pWindow = reinterpret_cast<Win32Window*>
			(GetWindowLongPtr(handle, GWLP_USERDATA));

		if (pWindow)
			pWindow->processEvent(message, wParam, lParam);

		return DefWindowProc(handle, message, wParam, lParam);
	}

	void Win32Window::minimize() {}
	void Win32Window::maximize() {}
	void Win32Window::show() {}
	void Win32Window::hide() {}
	bool Win32Window::isFullscreen() const { return m_config.fullscreen; }
	bool Win32Window::isOpen() const { return !m_config.closed; }
	void Win32Window::setFullscreen(bool fullscreen) {}
	IWindow::Size Win32Window::getDimensions() const { return{ m_config.width, m_config.height }; }
	void Win32Window::resize(unsigned int width, unsigned int height) {}

	void Win32Window::processEvent(UINT message, WPARAM wParam, LPARAM lParam)
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

	void Win32Window::OnResize(int width, int height)
	{

	}
}