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

		const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
		const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		const char* glslVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

		Logger::log(version);
		Logger::log(vendor);
		Logger::log(renderer);
		Logger::log(glslVersion);
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

	GLuint createVBO(size_t typeSize, uint32_t elementCount, const void* data)
	{
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, typeSize * elementCount, data, GL_STATIC_DRAW); // TODO(Phil): Check what GL_DYNAMIC_DRAW does																			  
																					  // TODO(Phil): Check for errors here.
		return vbo;
	}

	GLuint attribSizeToGl(AttributeSize::Value type)
	{
		static const GLuint glType[AttributeSize::Count] =
		{
			GL_DOUBLE,
			GL_FLOAT,
			GL_UNSIGNED_INT,
			GL_INT
		};

		return glType[type];
	}

	VertexBufferHandle WGlRenderContext::createVertexBuffer(VertexBufferFormat format)
	{
		m_vertexBuffers.emplace_back();
		GlVertexBuffer& buffer = m_vertexBuffers.back();

		VertexBufferHandle handle;
		handle.idx = static_cast<uint16_t>(m_vertexBuffers.size() - 1);

		glGenVertexArrays(1, &buffer.m_id);
		glBindVertexArray(buffer.m_id);

		size_t attributeCount = format.m_count;
		
		for (size_t i = 0; i < attributeCount; ++i)
		{
			const VertexFormat::Data& attribData = format.m_inputData[i];
			const VertexFormat::Decl& attribDecl = format.m_inputDecl[i];

			GLuint vbo = createVBO(attribData.m_size, attribData.m_count, attribData.m_data);
			
			glEnableVertexAttribArray(i);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glVertexAttribPointer(
				i, 
				attribDecl.m_numElements, 
				attribSizeToGl(attribDecl.m_size), 
				attribData.m_bNormalize, 
				attribData.m_size, 
				nullptr); 
		}

		// TODO(Phil): Check for errors here.

		return handle;
	}

	IndexBufferHandle WGlRenderContext::createIndexBuffer(size_t size, uint32_t count, const void* data)
	{
		m_indexBuffers.emplace_back();
		GlIndexBuffer& buffer = m_indexBuffers.back();

		IndexBufferHandle handle;
		handle.idx = static_cast<uint16_t>(m_indexBuffers.size() - 1);

		glGenBuffers(1, &buffer.m_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.m_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * count, data, GL_STATIC_DRAW); // TODO(Phil): Check what GL_DYNAMIC_DRAW does

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
		default:
		{
			Logger::error("Trying to convert invalid Shader::Type to GlEnum");
			return 0;
		}
		}
	}

	Shader::Type getShaderType(GLenum shaderEnum)
	{
		switch (shaderEnum)
		{
		case GL_VERTEX_SHADER:
		{
			return Shader::Type::Vertex;
		}
		case GL_FRAGMENT_SHADER:
		{
			return Shader::Type::Fragment;
		}
		case GL_COMPUTE_SHADER:
		{
			return Shader::Type::Compute;
		}
		case GL_GEOMETRY_SHADER:
		{
			return Shader::Type::Geometry;
		}
		default:
		{
			Logger::error("Trying to convert invalid GlEnum to Shader::Type ");
			return Shader::Type::None;
		}
		}
	}

	void printShaderLog(GLuint shader)
	{
		if (glIsShader(shader))
		{
			int infoLogLength = 0;
			int maxLength = infoLogLength;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			char* infoLog = new char[maxLength];

			glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);

			if (infoLogLength > 0)
			{
				Logger::error(infoLog);
			}

			delete[] infoLog;
		}

		else
		{
			Logger::error(std::to_string(shader) + " is not a shader\n");
		}
	}

	ShaderHandle WGlRenderContext::createShader(const char* source, Shader::Type shaderType)
	{
		m_shaders.emplace_back();
		GlShader& shader = m_shaders.back();

		ShaderHandle handle;

		if (source == nullptr)
		{
			Logger::error("Shader source is null");
			return handle;
		}

		handle.idx = static_cast<uint16_t>(m_shaders.size() - 1);

		shader.m_shaderType = getShaderEnum(shaderType);
		shader.m_id = glCreateShader(shader.m_shaderType);
		glShaderSource(shader.m_id, 1, (const char**)&source, NULL);
		glCompileShader(shader.m_id);

		printShaderLog(shader.m_id);
	
		return handle;
	}
	
	ProgramHandle WGlRenderContext::createProgram(const Shader::List& shaders)
	{
		m_programs.emplace_back();
		GlProgram& program = m_programs.back();

		GLuint progHandle = glCreateProgram();

		int typeValue = 0;

		for (const ShaderHandle& handle : shaders)
		{
			if (handle.isValid())
			{
				GlShader& shader = m_shaders[handle.idx];

				if (getShaderType(shader.m_shaderType) == typeValue)
				{
					glAttachShader(progHandle, shader.m_id);
				}
				else
				{
					Logger::error("Trying to attach shader of invalid type");
				}
			}
			else
			{			
				Logger::error("Trying to access invalid shader");
			}

			typeValue++;
		}

		glLinkProgram(progHandle);
		ProgramHandle handle;

		if (!glIsProgram(progHandle))
		{
			Logger::error("Failed to compile shader program.");
			return handle;
		}

		program.m_id = progHandle;
		handle.idx = static_cast<uint16_t>(m_programs.size() - 1);
		return handle;
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