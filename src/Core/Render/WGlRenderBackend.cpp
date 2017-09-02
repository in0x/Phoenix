#include "WGlRenderBackend.hpp"
#include "../OpenGL.hpp"
#include "../Logger.hpp"
#include <assert.h>

namespace Phoenix
{
	WGlRenderBackend::WGlRenderBackend()
		: m_owningWindow(nullptr)
		, m_renderContext(0)
	{
	}

	void WGlRenderBackend::WGlRenderBackend::init(RenderInit* initValues)
	{
		auto wglInitValues = dynamic_cast<WGlRenderInit*>(initValues);

		if (!wglInitValues)
		{
			Logger::error("Passed RenderInit of invalid type");
			assert(false);
		}

		m_owningWindow = wglInitValues->m_owningWindow;

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

	WGlRenderBackend::~WGlRenderBackend()
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_renderContext);
	}

	void WGlRenderBackend::WGlRenderBackend::swapBuffer()
	{
		SwapBuffers(m_deviceContext);
	}

	uint32_t WGlRenderBackend::getMaxTextureUnits()
	{
		GLint maxTextureUnits;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
		return static_cast<uint32_t>(maxTextureUnits);
	}

	uint32_t WGlRenderBackend::getMaxUniformCount()
	{
		GLint maxUniforms;
		glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &maxUniforms);
		return static_cast<uint32_t>(maxUniforms);
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

	GLuint attribSizeToGl(AttributeType::Value type)
	{
		static const GLuint glType[AttributeType::Count] =
		{
			GL_DOUBLE,
			GL_FLOAT,
			GL_UNSIGNED_INT,
			GL_INT
		};

		return glType[type];
	}

	void WGlRenderBackend::createVertexBuffer(VertexBufferHandle handle, const VertexBufferFormat& format)
	{
		GlVertexBuffer& buffer = m_vertexBuffers[handle.idx];
		glGenVertexArrays(1, &buffer.m_id);
		glBindVertexArray(buffer.m_id);

		size_t attribCount = format.size();
		for (size_t location = 0; location < attribCount; ++location)
		{
			const VertexAttrib::Data& data = format.at(location)->m_data;
			const VertexAttrib::Decl& decl = format.at(location)->m_decl;

			GLuint vbo = createVBO(data.m_size, data.m_count, data.m_data);

			glEnableVertexAttribArray(location);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glVertexAttribPointer(
				location,
				decl.m_numElements,
				attribSizeToGl(decl.m_type),
				data.m_bNormalize,
				data.m_size,
				nullptr);
		}
	}

	void WGlRenderBackend::createIndexBuffer(IndexBufferHandle handle, size_t size, uint32_t count, const void* data)
	{
		GlIndexBuffer& buffer = m_indexBuffers[handle.idx];

		glGenBuffers(1, &buffer.m_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.m_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * count, data, GL_STATIC_DRAW); // TODO(Phil): Check what GL_DYNAMIC_DRAW does
	};

	GLenum getShaderEnum(Shader::Type type)
	{
		static const GLuint glType[Shader::Type::Count] =
		{
			 GL_VERTEX_SHADER,
			 GL_GEOMETRY_SHADER,
			 GL_FRAGMENT_SHADER,
			 GL_COMPUTE_SHADER,
		};

		return glType[type];
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
			return Shader::Type::Count;
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

	void WGlRenderBackend::createShader(ShaderHandle handle, const char* source, Shader::Type shaderType)
	{
		GlShader& shader = m_shaders[handle.idx];

		if (source == nullptr)
		{
			Logger::error("Shader source is null");
			return;
		}

		shader.m_shaderType = getShaderEnum(shaderType);
		shader.m_id = glCreateShader(shader.m_shaderType);
		glShaderSource(shader.m_id, 1, (const char**)&source, NULL);
		glCompileShader(shader.m_id);

		printShaderLog(shader.m_id);
	}

	void WGlRenderBackend::createProgram(ProgramHandle handle, const Shader::List& shaders)
	{
		GlProgram& program = m_programs[handle.idx];

		GLuint progHandle = glCreateProgram();

		int typeCount = 0;

		for (const ShaderHandle& handle : shaders)
		{
			if (handle.isValid())
			{
				GlShader& shader = m_shaders[handle.idx];

				if (getShaderType(shader.m_shaderType) == typeCount)
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

			typeCount++;
		}

		glLinkProgram(progHandle);

		if (!glIsProgram(progHandle))
		{
			Logger::error("Failed to compile shader program.");
			return;
		}

		program.m_id = progHandle;
		return;
	}

	void WGlRenderBackend::createTexture()
	{
		Logger::warning(__LOCATION_INFO__ "not implemented!");
	}

	void WGlRenderBackend::createFrameBuffer()
	{
		Logger::warning(__LOCATION_INFO__ "not implemented!");
	}

	void getUniformSetter(Uniform::Type type)
	{

	}

	void WGlRenderBackend::createUniform(UniformHandle handle, const char* name, Uniform::Type type, const void* data)
	{
		Logger::warning(__LOCATION_INFO__ "not implemented!");

		// When creating a shader, get all info via introspection -> all uniform names. 
		// Create uniform info independently of context. Use shader info when bound to see what uniforms can be used

		// The user specifies which uniforms to bind using the StateGroup
	}

	void WGlRenderBackend::setUniform(UniformHandle handle, const void* data)
	{
		GlUniform uniform = m_uniforms[handle.idx];

		switch (uniform.m_type)
		{
		case Uniform::Float:
		{
			glUniform1fv(uniform.m_location, 1, static_cast<const GLfloat*>(data));
			break;
		}
		case Uniform::Int:
		{
			glUniform1iv(uniform.m_location, 1, static_cast<const GLint*>(data));
			break;
		}
		case Uniform::Vec3:
		{
			glUniform3fv(uniform.m_location, 1, static_cast<const GLfloat*>(data));
			break;
		}
		case Uniform::Vec4:
		{
			glUniform4fv(uniform.m_location, 1, static_cast<const GLfloat*>(data));
			break;
		}
		case Uniform::Mat3:
		{
			glUniformMatrix3fv(uniform.m_location, 1, false, static_cast<const GLfloat*>(data));
			break;
		}
		case Uniform::Mat4:
		{
			glUniformMatrix4fv(uniform.m_location, 1, false, static_cast<const GLfloat*>(data));
			break;
		}
		default:
		{
			Logger::error("Invalid uniform type used to set value");
			break;
		}
		};
	}

	void WGlRenderBackend::WGlRenderBackend::setVertexBuffer(VertexBufferHandle vb)
	{
		GlVertexBuffer buffer = m_vertexBuffers[vb.idx];
		glBindVertexArray(buffer.m_id);
	}

	void WGlRenderBackend::WGlRenderBackend::setIndexBuffer(IndexBufferHandle ib)
	{
		GlIndexBuffer buffer = m_indexBuffers[ib.idx];
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.m_id);
	}

	void WGlRenderBackend::WGlRenderBackend::setProgram(ProgramHandle prog)
	{
		GlProgram program = m_programs[prog.idx];
		glUseProgram(program.m_id);
	}

	void WGlRenderBackend::setDepth(Depth::Type depth)
	{
		Logger::warning(__LOCATION_INFO__ "not implemented!");
	}

	void WGlRenderBackend::setRaster(Raster::Type raster)
	{
		Logger::warning(__LOCATION_INFO__ "not implemented!");
	}

	void WGlRenderBackend::setBlend(Blend::Type blend)
	{
		Logger::warning(__LOCATION_INFO__ "not implemented!");
	}

	void WGlRenderBackend::setStencil(Stencil::Type stencil)
	{
		Logger::warning(__LOCATION_INFO__ "not implemented!");
	}

	GLenum getPrimitiveEnum(Primitive::Type type)
	{
		static const GLuint glType[3] =
		{
			GL_POINTS,
			GL_LINES,
			GL_TRIANGLES
		};

		return glType[type];
	}

	void WGlRenderBackend::WGlRenderBackend::drawLinear(Primitive::Type primitive, uint32_t count, uint32_t start)
	{
		glDrawArrays(getPrimitiveEnum(primitive), start, count);
	}

	void WGlRenderBackend::WGlRenderBackend::drawIndexed(Primitive::Type primitive, uint32_t count, uint32_t start)
	{
		glDrawElements(getPrimitiveEnum(primitive), count, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLubyte) * start));
	}
}