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
			Logger::error("Failed to initialize gl3w");
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

	VertexBufferHandle WGlRenderContext::createVertexBuffer(uint32_t size, const void* data)
	{
		m_vertexBuffers.emplace_back();
		GlVertexBuffer& buffer = m_vertexBuffers.back();

		VertexBufferHandle handle;
		handle.idx = static_cast<uint16_t>(m_vertexBuffers.size() - 1);

		glGenBuffers(1, &buffer.m_id);
		glBindBuffer(GL_ARRAY_BUFFER, buffer.m_id);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); // TODO(Phil): Check what GL_DYNAMIC_DRAW does

		// TODO(Phil): Check for errors here.

		return handle;
	}

	IndexBufferHandle WGlRenderContext::createIndexBuffer(uint32_t size, const void* data)
	{
		m_indexBuffers.emplace_back();
		GlIndexBuffer& buffer = m_indexBuffers.back();

		IndexBufferHandle handle;
		handle.idx = static_cast<uint16_t>(m_indexBuffers.size() - 1);

		glGenBuffers(1, &buffer.m_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.m_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); // TODO(Phil): Check what GL_DYNAMIC_DRAW does

		return handle;
	};

	GLenum getShaderEnum(Shader::Type type)
	{
		switch (type)
		{
		case Shader::Type::Vertex:
		{
			return GL_VERTEX_SHADER;
		}
		case Shader::Type::Fragment:
		{
			return GL_FRAGMENT_SHADER;
		}
		case Shader::Type::Compute:
		{
			return GL_COMPUTE_SHADER;
		}
		case Shader::Type::Geometry:
		{
			return GL_GEOMETRY_SHADER;
		}
		}
	}

	ShaderHandle WGlRenderContext::createShader(const char* source, Shader::Type shaderType)
	{
		m_shaders.emplace_back();
		GlShader& shader = m_shaders.back();

		ShaderHandle handle;

		if (source == nullptr)
		{
			Logger::Error("Shader source is null");
			return handle;
		}

		handle.idx = static_cast<uint16_t>(m_shaders.size() - 1);

		shader.m_id = glCreateShader(getShaderEnum(shaderType));
		glShaderSource(shader.m_id, 1, (const char**)&source, NULL);
		glCompileShader(shader.m_id);
	
		return handle;
	}
	
	ProgramHandle WGlRenderContext::createProgram() 
	{ 
		return{}; 
	}
	
	TextureHandle WGlRenderContext::createTexture() 
	{ 
		return{}; 
	}
	
	FrameBufferHandle WGlRenderContext::createFrameBuffer() 
	{ 
		return{}; 
	}
}