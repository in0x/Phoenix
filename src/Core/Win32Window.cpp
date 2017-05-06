#include "Win32Window.hpp"
#include "Logger.hpp"

namespace Phoenix
{
	unsigned int Win32Window::s_windowCount;

	Win32Window::Win32Window(const WindowConfig& config)
	{
		left = config.left;
		top = config.top;
		width = config.width;
		height = config.height;
		fullscreen = config.fullscreen;
		name = config.windowName;

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
		windowClass.lpszClassName = config.windowName.c_str();

		if (s_windowCount == 0)
		{
			RegisterClass(&windowClass);
		}

		DWORD exWindStyle = 0;
		DWORD windStyle = 0;

		RECT rect = { left,
			top,
			left + width,
			top + height };

		if (fullscreen)
		{
			DEVMODE displayConfig = {};
			memset(&displayConfig, 0, sizeof(displayConfig));
			displayConfig.dmSize = sizeof(displayConfig);
			displayConfig.dmPelsWidth = width;
			displayConfig.dmPelsHeight = height;
			displayConfig.dmBitsPerPel = 32;
			displayConfig.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

			if (ChangeDisplaySettings(&displayConfig, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				Logger::Error("Failed to fullscreen the window.");
				return;
			}

			exWindStyle = WS_EX_APPWINDOW;
			windStyle = WS_POPUP;
		}
		else
		{
			exWindStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			windStyle = WS_OVERLAPPEDWINDOW;

			// Reconfigures window rect to factor in size of border.
			AdjustWindowRectEx(&rect, windStyle, FALSE, exWindStyle);
		}

		window = CreateWindowEx(
			exWindStyle,
			name.c_str(),
			name.c_str(),
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | windStyle,
			left, top,
			width, height,
			nullptr, // Parent window
			nullptr,
			GetModuleHandle(nullptr),
			this);

		if (!window)
		{
			Logger::Error("Failed to create window.");
			return;
		}

		msg.message = ~WM_QUIT;
		ShowWindow(window, SW_SHOW);
		s_windowCount++;
	}

	Win32Window::~Win32Window()
	{
		if (fullscreen)
			ChangeDisplaySettings(nullptr, 0);

		DestroyWindow(window);
		UnregisterClass(name.c_str(), GetModuleHandle(nullptr));

		s_windowCount--;
	};

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

	void Win32Window::processEvent(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CLOSE:
			PostQuitMessage(0);
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

	void Win32Window::processMessages()
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void Win32Window::minimize()
	{
		Logger::Warning(__LOCATION_INFO__ "Not implemented!");
	}

	void Win32Window::maximize()
	{
		Logger::Warning(__LOCATION_INFO__ "Not implemented!");
	}

	void Win32Window::setVisible(bool visible)
	{
		ShowWindow(window, visible ? SW_SHOW : SW_HIDE);
	}

	bool Win32Window::isFullscreen() const
	{
		return fullscreen;
	}

	bool Win32Window::isOpen() const
	{
		Logger::Warning(__LOCATION_INFO__ "Not implemented!");
		return true;
	}

	void Win32Window::setFullscreen(bool fullscreen)
	{
		Logger::Warning(__LOCATION_INFO__ "Not implemented!");
	}

	Win32Window::Size Win32Window::getDimensions() const
	{
		return{ width, height };
	}

	void Win32Window::resize(unsigned int width, unsigned int height)
	{
		Logger::Warning(__LOCATION_INFO__ "Not implemented!");
	}

	void Win32Window::OnResize(int width, int height)
	{
		Logger::Warning(__LOCATION_INFO__ "Not implemented!");
	}
}