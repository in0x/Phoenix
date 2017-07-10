#include "WGlRenderContext.hpp"
#include "../OpenGL.hpp"
#include "../Logger.hpp"
#include <assert.h>

namespace Phoenix
{

	WGlRenderContext::WGlRenderContext(HWND owningWindow)
		: m_owningWindow(owningWindow)
		, m_renderContext(0)
	{
	}

	void WGlRenderContext::init()
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

		m_deviceContext = GetDC(m_owningWindow);
		int chosenPixelFormat = ChoosePixelFormat(m_deviceContext, &pfd);

		if (SetPixelFormat(m_deviceContext, chosenPixelFormat, &pfd))
		{
			// Try to set that pixel format
			m_renderContext = wglCreateContext(m_deviceContext);

			if (m_renderContext != 0)
			{
				wglMakeCurrent(m_deviceContext, m_renderContext);	// Make our render context current
				glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Black Background
				glClearDepth(1.0f);									// Depth Buffer Setup
				glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
																	// Ok do this later.
			}
		}

		//ReleaseDC(owningWindow, deviceContext);	// Release the window device context now that we are done

		GLenum err = glewInit(); // I need to do this when a context is created / made current for the first time.
		if (err != GLEW_OK)
		{
			Logger::Error("Failed to initialize gl3w");
			assert(false);
		}
	}

	WGlRenderContext::~WGlRenderContext()
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_renderContext);
	}

	void WGlRenderContext::swapBuffer()
	{
		SwapBuffers(m_deviceContext);
	}


	VertexBufferHandle WGlRenderContext::createVertexBuffer() 
	{
		VertexBufferHandle handle;
		GLuint vbo;

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * fox->vertices.size(), fox->vertices.data(), GL_STATIC_DRAW);
	}

	IndexBufferHandle WGlRenderContext::createIndexBuffer() {};
	ShaderHandle WGlRenderContext::createShader() {}
	ProgramHandle WGlRenderContext::createProgram() {}
	TextureHandle WGlRenderContext::createTexture() {}
	FrameBufferHandle WGlRenderContext::createFrameBuffer() {}
}