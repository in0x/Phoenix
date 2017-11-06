#pragma once

#include "../RIDevice.hpp"
#include "../RIContext.hpp"
#include "../RenderWindow.hpp" 
// NOTE(Phil): These are included instead of forward declared so I can just include
// this header for starting and using the render system.

namespace Phoenix
{
	class RIOpenGL
	{
	public:
		RIOpenGL();
		~RIOpenGL();
		
		bool init();

		void exit();

		RenderWindow* createWindow(const WindowConfig& config);

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