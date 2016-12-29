#pragma once

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

		virtual bool init() = 0;
		virtual void minimize() = 0;
		virtual void maximize() = 0;
		virtual void show() = 0;
		virtual void hide() = 0;
		virtual bool isFullscreen() const = 0;
		virtual bool isOpen() const = 0;
		virtual void setFullscreen(bool fullscreen) = 0;
		virtual Size getDimensions() const = 0;
		virtual void resize(unsigned int width, unsigned int height) = 0;
	};
}