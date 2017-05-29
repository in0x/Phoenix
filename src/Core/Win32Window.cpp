#include "Win32Window.hpp"
#include "Logger.hpp"

namespace Phoenix
{
	struct Win32Window::Pimpl
	{
		unsigned int left;
		unsigned int top;
		unsigned int width;
		unsigned int height;
		bool fullscreen;
		HWND window;
		MSG msg;
		std::wstring name;

		~Pimpl()
		{
			if (!window)
				return;

			if (fullscreen)
				ChangeDisplaySettings(nullptr, 0);

			DestroyWindow(window);
			UnregisterClass(name.c_str(), GetModuleHandle(nullptr));
		}
	};

	unsigned int Win32Window::s_windowCount;

	Win32Window::Win32Window(const WindowConfig& config)
	{
		self = std::make_unique<Pimpl>();
		self->left = config.left;
		self->top = config.top;
		self->width = config.width;
		self->height = config.height;
		self->fullscreen = config.fullscreen;
		self->name = config.windowName;

		WNDCLASS windowClass;

		windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
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
			if (!RegisterClass(&windowClass))
			{
				Logger::Error("Failed to register the window class.");
				self = nullptr;
				return;
			}
		}

		DWORD exWindStyle = 0;
		DWORD windStyle = 0;

		RECT rect = { self->left,
					  self->top,
					  self->left + self->width,
					  self->top  + self->height };

		if (self->fullscreen)
		{
			DEVMODE displayConfig;
			memset(&displayConfig, 0, sizeof(displayConfig));
		/*	displayConfig.dmSize = sizeof(displayConfig);
			displayConfig.dmPelsWidth = self->width;
			displayConfig.dmPelsHeight = self->height;
			displayConfig.dmBitsPerPel = 32;
			displayConfig.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;*/

			EnumDisplaySettings(nullptr, ENUM_REGISTRY_SETTINGS, &displayConfig);

			if (ChangeDisplaySettings(&displayConfig, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				Logger::Error("Failed to fullscreen the window.");
				self = nullptr;
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

		self->window = CreateWindowEx(
			exWindStyle,
			self->name.c_str(),
			self->name.c_str(),
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | windStyle,
			self->left, self->top,
			self->width, self->height,
			nullptr, // Parent window
			nullptr,
			GetModuleHandle(nullptr),
			this);

		if (!self->window)
		{
			Logger::Error("Failed to create window.");
			self = nullptr;
			return;
		}

		self->msg.message = ~WM_QUIT;
		ShowWindow(self->window, SW_SHOW);
		s_windowCount++;
	}

	Win32Window::~Win32Window()
	{
		self = nullptr;
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
			self = nullptr;
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
		if (PeekMessage(&self->msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&self->msg);
			DispatchMessage(&self->msg);
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
		ShowWindow(self->window, visible ? SW_SHOW : SW_HIDE);
	}

	bool Win32Window::isFullscreen() const
	{
		return self->fullscreen;
	}

	bool Win32Window::isOpen() const
	{
		return self != nullptr;
	}

	void Win32Window::setFullscreen(bool fullscreen)
	{
		Logger::Warning(__LOCATION_INFO__ "Not implemented!");
	}

	Win32Window::Size Win32Window::getDimensions() const
	{
		return{ self->width, self->height };
	}

	void Win32Window::resize(unsigned int width, unsigned int height)
	{
		Logger::Warning(__LOCATION_INFO__ "Not implemented!");
	}

	HWND Win32Window::getNativeHandle()
	{
		return self->window;
	}

	void Win32Window::OnResize(int width, int height)
	{
		Logger::Warning(__LOCATION_INFO__ "Not implemented!");
	}
}