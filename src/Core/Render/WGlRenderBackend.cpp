#include "WGlRenderBackend.hpp"
#include "UniformStore.hpp"

#include "../OpenGL.hpp"
#include "../Logger.hpp"
#include "../Math/PhiMath.hpp"
#include "../glew/wglew.h"
#include "../Windows/Win32Window.hpp"

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
		UniformList boundUniforms;
		TextureList boundTextures;
	};

	WGlRenderBackend::WGlRenderBackend()
		: m_owningWindow(nullptr)
		, m_renderContext(0)
		, m_uniformCount(0)
		, m_msaaSupport(0)
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

		initGlew();
		checkMsaaSupport(*wglInitValues);

		if (wglInitValues->m_msaaSamples > 0 && m_msaaSupport > 0)
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
	}

	void WGlRenderBackend::checkMsaaSupport(const WGlRenderInit& initValues)
	{
		if (!glewIsSupported("GL_ARB_multisample"))
		{
			m_msaaSupport = 0;
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
				m_msaaSupport = maxMsaaSupport;
				return;
			}
		}
	}

	void WGlRenderBackend::initBasic(const WGlRenderInit& initValues)
	{
		m_owningWindow = initValues.m_owningWindow;

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

	void WGlRenderBackend::initGlew()
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

	void WGlRenderBackend::initWithMSAA(const WGlRenderInit& initValues)
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

		m_owningWindow = initValues.m_owningWindow;
		m_deviceContext = GetDC(m_owningWindow);
		wglChoosePixelFormatARB(m_deviceContext, attributes, nullptr, 1, &chosenPixelFormat, &numPixelCounts);

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
		for (GLuint location = 0; location < attribCount; ++location)
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

	GLenum toGlFormat(ETextureFormat::Type format)
	{
		static GLenum formats[] = { GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_CUBE_MAP };
		return formats[format];
	}

	GLint toGlComponents(ETextureComponents::Type components)
	{
		static GLenum glComponents[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA, GL_DEPTH_COMPONENT };
		return glComponents[components];
	}

	void WGlRenderBackend::createTexture(TextureHandle handle, const TextureDesc& description, const char* name)
	{
		GlTexture& tex = m_textures[handle.idx];

		GLenum format = toGlFormat(description.format);
		GLenum components = toGlComponents(description.components);
		
		tex.m_format = format;

		glGenTextures(1, &tex.m_id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(format, tex.m_id);

		glTexImage2D(format,
					0,
					components,
					description.width, description.height,
					0,
					components,
					GL_UNSIGNED_BYTE,
					description.data);

		glTexParameteri(format, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(format, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(format, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(format, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	void WGlRenderBackend::createFrameBuffer()
	{
		Logger::warning(__LOCATION_INFO__ "not implemented!");
	}

	// NOTE(Phil): Textures return type int because the locations map to 
	// texture units.
	EUniform::Type toUniformType(GLenum glType)
	{
		switch (glType)
		{
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
		case GL_INT:
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		{
			return EUniform::Int;
		} break;
		case GL_FLOAT_MAT4:
		{
			return EUniform::Mat4;
		} break;
		case GL_FLOAT:
		{
			return EUniform::Float;
		} break;
		default:
		{
			Logger::error("Trying to convert invalid GlEnum to Uniform::Type");
			assert(false);
			return EUniform::Count;
		} break;
		}
	}

	bool checkIsBasicUniform(GLenum typeVal)
	{
		return (typeVal == GL_FLOAT
			 || typeVal == GL_INT
			 || typeVal == GL_FLOAT_VEC3
			 || typeVal == GL_FLOAT_VEC4
			 || typeVal == GL_FLOAT_MAT3
			 || typeVal == GL_FLOAT_MAT4);
	}

	bool checkIsSampler(GLenum typeVal)
	{
		return (typeVal == GL_TEXTURE_2D
			 || typeVal == GL_TEXTURE_3D
			 || typeVal == GL_TEXTURE_CUBE_MAP
			 || typeVal == GL_TEXTURE_1D);
	}

	UniformHandle WGlRenderBackend::addUniform()
	{
		UniformHandle handle = createUniformHandle();
		handle.idx = m_uniformCount++;
		return handle;
	}

	void WGlRenderBackend::registerActiveUniforms(ProgramHandle programHandle) // Might be able to store texture using ::Int, seems to store Texture Slot
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

	void WGlRenderBackend::drawLinear(VertexBufferHandle vertexbuffer, EPrimitive::Type primitive, uint32_t count, uint32_t start)
	{
		setVertexBuffer(vertexbuffer);
		glDrawArrays(getPrimitiveEnum(primitive), start, count);
	}

	void WGlRenderBackend::drawIndexed(VertexBufferHandle vertexBuffer, IndexBufferHandle indexBuffer, EPrimitive::Type primitive, uint32_t count, uint32_t start)
	{
		setVertexBuffer(vertexBuffer);
		setIndexBuffer(indexBuffer);
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