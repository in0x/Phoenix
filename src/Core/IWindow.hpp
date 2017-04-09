#pragma once

#include "PhiCoreRequired.hpp"
#include "Logger.hpp"

namespace Phoenix
{
	struct WindowConfig;

	class IWindow
	{
	public:
		struct Size
		{
			unsigned int width;
			unsigned int height;
		};

		IWindow() {}
		IWindow(IWindow&) = delete;
		IWindow(IWindow&&) = delete;
		virtual ~IWindow() {}

		virtual void minimize() = 0;
		virtual void maximize() = 0;
		virtual void setVisible(bool visible) = 0;
		virtual bool isFullscreen() const = 0;
		virtual bool isOpen() const = 0;
		virtual void setFullscreen(bool fullscreen) = 0;
		virtual Size getDimensions() const = 0;
		virtual void resize(unsigned int width, unsigned int height) = 0;
	};
}