#pragma once

#include <Windows.h>

namespace Phoenix
{
	class Win32GLContext
	{
	public:
		Win32GLContext(HWND owningWindow);
		~Win32GLContext();
		void SwapBuffer();
	
	private:
		HGLRC m_renderContext;
		HDC m_deviceContext;
	};
}