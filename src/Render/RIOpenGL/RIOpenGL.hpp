#pragma once

#include <Render/RIDevice.hpp>
#include <Render/RIContext.hpp>
#include <Render/RenderWindow.hpp> 

#include <memory>

namespace Phoenix
{
	class RIOpenGL
	{
	public:
		RIOpenGL();
		~RIOpenGL();
		
		bool init();

		void exit();

		// NOTE(Phil): The unique_ptr here is a fix for some broken code I had before.
		// I need to rework how platform windows are handed out, but right now I only
		// have time to do a real quick fix.
		std::unique_ptr<RenderWindow> RIOpenGL::createWindow(const WindowConfig& config);

		void setWindowToRenderTo(RenderWindow* window);

		void swapBufferToWindow(RenderWindow* window);
		
		IRIDevice* getRenderDevice();
		
		IRIContext* getRenderContex();		

	private:
		class Impl;
		Impl* m_pImpl;
	};

	namespace Platform
	{
		void pollEvents();
	}
}