#pragma once

#include <stdint.h>

namespace Phoenix
{
	struct WindowConfig
	{
		uint32_t width;
		uint32_t height;
		const char* title;
		bool bFullscreen;
	};

	class RenderWindow
	{
	public:
		virtual ~RenderWindow() {}

		virtual bool wantsToClose() { return false; }
	};
}