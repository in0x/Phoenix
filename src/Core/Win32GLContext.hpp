#pragma once

#include <Windows.h>
#include "OpenGL.hpp"

namespace Phoenix
{
	class Win32GLContext
	{
	public:
		Win32GLContext(HWND owningWindow)
			: m_renderContext(0)
		{
			PIXELFORMATDESCRIPTOR pfd = 
			{
				sizeof(PIXELFORMATDESCRIPTOR),
				1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //Flags
				PFD_TYPE_RGBA,												// The kind of framebuffer. RGBA or palette.
				32,															// Colordepth of the framebuffer.
				0, 0, 0, 0, 0, 0,
				0,
				0,
				0,
				0, 0, 0, 0,
				24,															// Number of bits for the depthbuffer
				8,															// Number of bits for the stencilbuffer
				0,															// Number of Aux buffers in the framebuffer.
				PFD_MAIN_PLANE,
				0,
				0, 0, 0
			};

			m_deviceContext = GetDC(owningWindow);
			int chosenPixelFormat = ChoosePixelFormat(m_deviceContext, &pfd);
			
			if (SetPixelFormat(m_deviceContext, chosenPixelFormat, &pfd))
			{ 
				// Try to set that pixel format
				m_renderContext = wglCreateContext(m_deviceContext);
				
				if (m_renderContext != 0) 
				{
					wglMakeCurrent(m_deviceContext, m_renderContext);		// Make our render context current
					glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
					glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Black Background
					glClearDepth(1.0f);									// Depth Buffer Setup
					glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
					
					// Ok do this later.
				}
			}
			
			//ReleaseDC(owningWindow, deviceContext);	// Release the window device context now that we are done
		}

		void SwapBuffer()
		{
			SwapBuffers(m_deviceContext);
		}

		~Win32GLContext()
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(m_renderContext);
		}
	
	private:
		HGLRC m_renderContext;
		HDC m_deviceContext;
	};
}