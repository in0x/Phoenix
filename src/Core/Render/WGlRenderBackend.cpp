#include "WGlRenderBackend.hpp"
#include "UniformStore.hpp"

#include "../OpenGL.hpp"
#include "../Logger.hpp"
#include "../Math/PhiMath.hpp"

#include <assert.h>

namespace Phoenix
{
#define glChecked(x) x; checkGlError();\

	struct State
	{
		VertexBufferHandle vb;
		IndexBufferHandle ib;
		ProgramHandle program;
		EDepth::Type depth;
		ERaster::Type raster;
		EBlend::Type blend;
		EStencil::Type stencil;
		ResourceList<UniformHandle> boundUniforms;
	};

	WGlRenderBackend::WGlRenderBackend()
		: m_owningWindow(nullptr)
		, m_renderContext(0)
		, m_uniformCount(0)
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

	uint32_t WGlRenderBackend::getMaxTextureUnits() const
	{
		GLint maxTextureUnits;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
		return static_cast<uint32_t>(maxTextureUnits);
	}

	uint32_t WGlRenderBackend::getMaxUniformCount() const
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

	GLuint attribSizeToGl(EAttributeType::Value type)
	{
		static const GLuint glType[EAttributeType::Count] =
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

	GLenum getShaderEnum(EShader::Type type)
	{
		static const GLuint glType[EShader::Type::Count] =
		{
			 GL_VERTEX_SHADER,
			 GL_GEOMETRY_SHADER,
			 GL_FRAGMENT_SHADER,
			 GL_COMPUTE_SHADER,
		};

		return glType[type];
	}

	EShader::Type getShaderType(GLenum shaderEnum)
	{
		switch (shaderEnum)
		{
		case GL_VERTEX_SHADER:
		{
			return EShader::Type::Vertex;
		}
		case GL_FRAGMENT_SHADER:
		{
			return EShader::Type::Fragment;
		}
		case GL_COMPUTE_SHADER:
		{
			return EShader::Type::Compute;
		}
		case GL_GEOMETRY_SHADER:
		{
			return EShader::Type::Geometry;
		}
		default:
		{
			Logger::error("Trying to convert invalid GlEnum to Shader::Type ");
			assert(false);
			return EShader::Type::Count;
		}
		}
	}

	// Returns wether an error occured during shader compilation.
	bool getAndLogShaderLog(GLuint shader)
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
				return true;
			}

			delete[] infoLog;
		}

		else
		{
			Logger::errorf("ID %d is not a shader", shader);
			return true;
		}

		return false;
	}

	void WGlRenderBackend::createShader(ShaderHandle handle, const char* source, EShader::Type shaderType)
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

		if (getAndLogShaderLog(shader.m_id))
		{
			// NOTE(Phil): Set handle invalid. Use references for handles.
			assert(false);
		}
	}

	void WGlRenderBackend::createProgram(ProgramHandle handle, const EShader::List& shaders)
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
		registerActiveUniforms(handle);
	}

	void WGlRenderBackend::createTexture()
	{
		Logger::warning(__LOCATION_INFO__ "not implemented!");
	}

	void WGlRenderBackend::createFrameBuffer()
	{
		Logger::warning(__LOCATION_INFO__ "not implemented!");
	}

	EUniform::Type toUniformType(GLenum glType)
	{
		switch (glType)
		{
		case GL_FLOAT:
		{
			return EUniform::Float;
		} break;
		case GL_INT:
		{
			return EUniform::Int;
		} break;
		case GL_FLOAT_VEC3:
		{
			return EUniform::Vec3;
		} break;
		case GL_FLOAT_VEC4:
		{
			return EUniform::Vec4;
		} break;
		case GL_FLOAT_MAT3:
		{
			return EUniform::Mat3;
		} break;
		case GL_FLOAT_MAT4:
		{
			return EUniform::Mat4;
		} break;
		default:
		{
			Logger::error("Trying to convert invalid GlEnum to Uniform::Type");
			return EUniform::Count;
			assert(false);
		} break;
		}
	}

	UniformHandle WGlRenderBackend::addUniform()
	{
		UniformHandle handle = createUniformHandle();
		handle.idx = m_uniformCount++;
		return handle;
	}

	void WGlRenderBackend::registerActiveUniforms(ProgramHandle programHandle)
	{
		GlProgram& program = m_programs[programHandle.idx];

		GLint count;
		GLint size;
		GLenum type;

		const GLsizei bufSize = RenderConstants::c_maxUniformNameLenght;
		GLchar name[bufSize];
		GLsizei length;

		glGetProgramiv(program.m_id, GL_ACTIVE_UNIFORMS, &count);

		for (GLint i = 0; i < count; ++i)
		{
			glGetActiveUniform(program.m_id, (GLuint)i, bufSize, &length, &size, &type, name);

			UniformHandle handle = addUniform();

			GlUniform& uniform = m_uniforms[handle.idx];
			uniform.m_type = toUniformType(type);
			uniform.m_location = i;
			uniform.m_size = 1;

			HashFNVIterative<> hash;
			hash.add(name, strlen(name));
			hash.add(&programHandle.idx, sizeof(decltype(programHandle.idx)));
			
			m_uniformMap[hash()] = handle;
		}
	}

	void WGlRenderBackend::createUniform(UniformHandle& uniformHandle, const char* name, EUniform::Type type)
	{
	}


	void WGlRenderBackend::setUniform(ProgramHandle programHandle, UniformHandle uniformHandle, const void* data) 
	{
		GlUniform& uniform = m_uniforms[uniformHandle.idx];
		
		switch (uniform.m_type)
		{
		case EUniform::Mat4:
		{
			const Matrix4* m = static_cast<const Matrix4*>(data);
			glUniformMatrix4fv(uniform.m_location, uniform.m_size, false, static_cast<const GLfloat*>(data));
		} break;
		case EUniform::Vec3:
		{
			glUniform3fv(uniform.m_location, uniform.m_size, static_cast<const GLfloat*>(data));
			checkGlError();
		} break;
		case EUniform::Vec4:
		{
			glUniform4fv(uniform.m_location, uniform.m_size, static_cast<const GLfloat*>(data));
		} break;

		case EUniform::Mat3:
		{
			glUniformMatrix3fv(uniform.m_location, uniform.m_size, false, static_cast<const GLfloat*>(data));
		} break;
		case EUniform::Float:
		{
			glUniform1fv(uniform.m_location, uniform.m_size, static_cast<const GLfloat*>(data));
		} break;
		case EUniform::Int:
		{
			glUniform1iv(uniform.m_location, uniform.m_size, static_cast<const GLint*>(data));
		} break;
		default:
		{
			Logger::error("Invalid uniform type used to set value");
			assert(false);
		} break;
		};

		checkGlError();
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

	void WGlRenderBackend::setDepth(EDepth::Type depth)
	{
		switch (depth)
		{
		case EDepth::Enable:
		{
			glEnable(GL_DEPTH_TEST);
		} break;
		case EDepth::Disable:
		{
			glDisable(GL_DEPTH_TEST);
		} break;
		default: break;
		}
	}

	void WGlRenderBackend::setRaster(ERaster::Type raster)
	{
		Logger::warning(__LOCATION_INFO__ "not implemented!");
	}

	void WGlRenderBackend::setBlend(EBlend::Type blend)
	{
		Logger::warning(__LOCATION_INFO__ "not implemented!");
	}

	void WGlRenderBackend::setStencil(EStencil::Type stencil)
	{
		Logger::warning(__LOCATION_INFO__ "not implemented!");
	}

	GLenum getPrimitiveEnum(EPrimitive::Type type)
	{
		static const GLuint glType[] =
		{
			GL_POINTS,
			GL_LINES,
			GL_TRIANGLES
		};

		return glType[type];
	}

	void WGlRenderBackend::bindUniforms(ProgramHandle boundProgram, const UniformInfo* uniforms, size_t count)
	{
		// NOTE(Phil): At some point, the bound uniforms should be cached so that we don't constantly
		// recopy their data.

		for (size_t i = 0; i < count; ++i)
		{
			const UniformInfo& info = uniforms[i];
			Hash name = info.nameHash;
			Hash hashWithProgram = hashBytes(&boundProgram.idx, sizeof(decltype(ProgramHandle::idx)), name);
			
			if (m_uniformMap.find(hashWithProgram) != m_uniformMap.end())
			{
				UniformHandle handle = m_uniformMap[hashWithProgram];
				
				if (m_uniforms[handle.idx].m_type == info.type)
				{
					setUniform(boundProgram, handle, info.data);
				}
				else
				{
					Logger::error("Uniform exists in program, but type is mismatched");
				}
			}
			else
			{
				Logger::error("Uniform does not exist in specified program");
			}
		}

		checkGlError();
	}

	void WGlRenderBackend::setState(const CStateGroup& state)
	{
		setProgram(state.program);
		setDepth(state.depth);
		bindUniforms(state.program, state.uniforms, state.uniformCount);
		//setBlend(state.blend);
		//setStencil(state.stencil);
		//setRaster(state.raster);
	}

	void WGlRenderBackend::drawLinear(EPrimitive::Type primitive, uint32_t count, uint32_t start)
	{
		glDrawArrays(getPrimitiveEnum(primitive), start, count);
	}

	void WGlRenderBackend::drawIndexed(EPrimitive::Type primitive, uint32_t count, uint32_t start)
	{
		glDrawElements(getPrimitiveEnum(primitive), count, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLubyte) * start));
	}

	void WGlRenderBackend::clearFrameBuffer(FrameBufferHandle handle, EBuffer::Type bitToClear, RGBA clearColor)
	{
		// TODO(PHIL): use actual frambuffer here, 0 is default for window
		
		switch (bitToClear)
		{
		case EBuffer::Color:
		{ 		
			glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clearColor);
		} break;
		case EBuffer::Depth:
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		} break;
		case EBuffer::Stencil:
		{ 
			glClear(GL_STENCIL_BUFFER_BIT);
		} break;
		}
	}
}