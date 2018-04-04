#pragma once

#include <Render/RIDevice.hpp>
#include <Render/RIContext.hpp>
#include <Render/RenderWindow.hpp> 

#include <memory>

namespace Phoenix
{
	namespace RI
	{
		bool init();

		void setWindowToRenderTo(RenderWindow* window);

		void swapBufferToWindow(RenderWindow* window);

		RenderWindow* createWindow(const WindowConfig& config);

		void destroyWindow(RenderWindow* window);

		IRIDevice* getRenderDevice();

		IRIContext* getRenderContex();

		void exit();
	};

	namespace Platform
	{
		void pollEvents();
	}
}