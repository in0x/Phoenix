#pragma once

#include "RIWGLGlewSupport.hpp"
#include "RenderInitWGL.hpp"
#include "OpenGL.hpp"

#include "../../Logger.hpp"
#include "../../Windows/Win32Window.hpp"
#include "../../Windows/PhiWindowsInclude.hpp"

#include <stdint.h>
#include <assert.h>
#include "../../glew/wglew.h"

namespace Phoenix
{
	struct RIWGLGlewSupport::Details
	{
		Details()
			: owningWindow(nullptr)
			, renderContext(0)
			, deviceContext(0)
			, msaaSupport(0)
		{}

		HWND owningWindow;
		HGLRC renderContext;
		HDC deviceContext;
		uint8_t msaaSupport;
	};

	RIWGLGlewSupport::RIWGLGlewSupport(RenderInit* initValues)
	{
		auto wglInitValues = dynamic_cast<RenderInitWGL*>(initValues);

		if (!wglInitValues)
		{
			Logger::error("Passed RenderInit of invalid type");
			assert(false);
		}

		m_details = new RIWGLGlewSupport::Details;

		initGlew();

		checkMsaaSupport(*wglInitValues);

		if (wglInitValues->m_msaaSamples > 0 && m_details->msaaSupport > 0)
		{
			initWithMSAA(*wglInitValues);
		}
		else
		{
			initBasic(*wglInitValues);
		}

		const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
		const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		const char* glslVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

		Logger::log(version);
		Logger::log(vendor);
		Logger::log(renderer);
		Logger::log(glslVersion);

		GLint majorVersion = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);

		if (majorVersion < 4)
		{
			Logger::errorf("Phoenix currently assumes atleast GL Version 4. Detected Version: %d.", majorVersion);
			assert(false);
		}
	}

	RIWGLGlewSupport::~RIWGLGlewSupport()
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_details->renderContext);
		delete m_details;
	}

	void RIWGLGlewSupport::swapBuffers()
	{
		SwapBuffers(m_details->deviceContext);
	}

	void RIWGLGlewSupport::checkMsaaSupport(const RenderInitWGL& initValues)
	{
		if (!glewIsSupported("GL_ARB_multisample"))
		{
			m_details->msaaSupport = 0;
			return;
		}

		HDC hDC = GetDC(initValues.m_owningWindow);

		int pixelFormat;
		int valid;
		UINT numFormats;
		float fAttributes[] = { 0,0 };

		int iAttributes[] = { WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
			WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB,24,
			WGL_ALPHA_BITS_ARB,8,
			WGL_DEPTH_BITS_ARB,16,
			WGL_STENCIL_BITS_ARB,0,
			WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
			WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
			WGL_SAMPLES_ARB, 32,
			0,0 };

		for (uint8_t maxMsaaSupport = 32; maxMsaaSupport >= 2; maxMsaaSupport /= 2)
		{
			iAttributes[19] = maxMsaaSupport;
			valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);

			if ((valid != 0) && numFormats >= 1)
			{
				m_details->msaaSupport = maxMsaaSupport;
				return;
			}
		}
	}

	void RIWGLGlewSupport::initBasic(const RenderInitWGL& initValues)
	{
		m_details->owningWindow = initValues.m_owningWindow;

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

		m_details->deviceContext = GetDC(m_details->owningWindow);
		int chosenPixelFormat = ChoosePixelFormat(m_details->deviceContext, &pfd);

		if (SetPixelFormat(m_details->deviceContext, chosenPixelFormat, &pfd))
		{
			// Try to set that pixel format
			m_details->renderContext = wglCreateContext(m_details->deviceContext);

			if (m_details->renderContext != 0)
			{
				wglMakeCurrent(m_details->deviceContext, m_details->renderContext);	// Make our render context current
				glEnable(GL_TEXTURE_2D);												// Enable Texture Mapping
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);									// Black Background
				glClearDepth(1.0f);														// Depth Buffer Setup
				glEnable(GL_DEPTH_TEST);												
			}
			else
			{
				Logger::error("Creating WGlContext failed.");
				assert(false);
			}
		}
		else
		{
			Logger::error("Failed to set PixelFormat when creating WGlContext.");
			assert(false);
		}
	}

	void RIWGLGlewSupport::initGlew()
	{
		WindowConfig config = {
			800, 600,
			0,0,
			std::wstring(L"Dummy"),
			false,
			false };

		Win32Window dummyWindow(config);

		if (!dummyWindow.isOpen())
		{
			Logger::error("Failed to initialize Win32Window");
			assert(false);
		}

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

		HDC deviceCtx = GetDC(dummyWindow.getNativeHandle());
		int chosenPixelFormat = ChoosePixelFormat(deviceCtx, &pfd);
		HGLRC renderCtx = 0;

		if (SetPixelFormat(deviceCtx, chosenPixelFormat, &pfd))
		{
			// Try to set that pixel format
			renderCtx = wglCreateContext(deviceCtx);

			if (renderCtx != 0)
			{
				wglMakeCurrent(deviceCtx, renderCtx);	// Make our render context current
			}
			else
			{
				Logger::error("Creating WGlContext failed.");
				assert(false);
			}
		}
		else
		{
			Logger::error("Failed to set PixelFormat when creating WGlContext.");
			assert(false);
		}

		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			Logger::error("Failed to initialize gl3w");
			assert(false);
		}

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(renderCtx);
		ReleaseDC(dummyWindow.getNativeHandle(), deviceCtx);
	}

	void RIWGLGlewSupport::initWithMSAA(const RenderInitWGL& initValues)
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

		int chosenPixelFormat = 0;
		UINT numPixelCounts = 0;

		int attributes[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_SAMPLE_BUFFERS_ARB, 1,
			WGL_SAMPLES_ARB, initValues.m_msaaSamples,
			0
		};

		m_details->owningWindow = initValues.m_owningWindow;
		m_details->deviceContext = GetDC(m_details->owningWindow);
		wglChoosePixelFormatARB(m_details->deviceContext, attributes, nullptr, 1, &chosenPixelFormat, &numPixelCounts);

		if (SetPixelFormat(m_details->deviceContext, chosenPixelFormat, &pfd))
		{
			// Try to set that pixel format
			m_details->renderContext = wglCreateContext(m_details->deviceContext);

			if (m_details->renderContext != 0)
			{
				wglMakeCurrent(m_details->deviceContext, m_details->renderContext);	// Make our render context current
				glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Black Background
				glClearDepth(1.0f);									// Depth Buffer Setup
				glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
				glEnable(GL_MULTISAMPLE);							// Enable MSAA
			}
			else
			{
				Logger::error("Creating WGlContext failed.");
				assert(false);
			}
		}
		else
		{
			Logger::error("Failed to set PixelFormat when creating WGlContext.");
			assert(false);
		}
	}
}
