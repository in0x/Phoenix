#pragma once

#include "PhiCoreRequired.hpp"
#include <Windows.h>

namespace Phoenix
{
	class Win32GLContext
	{
	public:

	private:
		HDC m_deviceContext;
		HGLRC m_renderContext;
	};
}