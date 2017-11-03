#pragma once

#include "../RiDefs.hpp"
#include "Windows.h" // NOTE(Phil): We should still eliminate this.

namespace Phoenix
{
	class RenderInitWGL : public RenderInit
	{
	public:
		explicit RenderInitWGL(HWND owningWindow, uint8_t msaaSamples = 0)
			: RenderInit(ERenderApi::Gl)
			, m_owningWindow(owningWindow)
			, m_msaaSamples(msaaSamples)
		{
		}

		HWND m_owningWindow;
		uint8_t m_msaaSamples;
	};
}