#include "WGlRenderBackend.hpp"
#include "UniformStore.hpp"

#include "../OpenGL.hpp"
#include "../Logger.hpp"
#include "../glew/wglew.h"
#include "../Windows/Win32Window.hpp"

#include <assert.h>

namespace Phoenix
{
#define glChecked(x) x; checkGlError();\

	WGlRenderBackend::WGlRenderBackend()
		: m_owningWindow(nullptr)
		, m_renderContext(0)
		, m_uniformCount(0)
		, m_msaaSupport(0)
		, m_activeTextures(0)
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
					
		checkGlError();
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
				Logger::error("Trying to access shader that has not been set.");
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

	GLenum toGlFormat(ETexture::Format format)
	{
		static GLenum formats[] = { GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_CUBE_MAP };
		return formats[format];
	}

	GLint toGlComponents(ETexture::Components components)
	{
		static GLenum glComponents[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA, GL_DEPTH_COMPONENT };
		return glComponents[components];
	}

	GLenum toGlFilter(ETexture::Filter filter)
	{
		static GLenum glFilters[] = { GL_NEAREST, GL_LINEAR };
		return glFilters[filter];
	}

	void WGlRenderBackend::uploadTextureData(TextureHandle handle, const void* data, uint32_t width, uint32_t height) 
	{
		GlTexture& tex = m_textures[handle.textureIdx];
		tex.m_dataType = GL_UNSIGNED_BYTE;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(tex.m_format, tex.m_id);

		GLenum format = tex.m_format;

		if (tex.m_format == GL_TEXTURE_CUBE_MAP)
		{
			format = GL_TEXTURE_CUBE_MAP_POSITIVE_X + tex.m_cubeface;
			tex.m_cubeface++;
		}

		glTexImage2D(format,
			0,
			tex.m_components,
			width, height,
			0,
			tex.m_components,
			tex.m_dataType,
			data);

		checkGlError();
	}

	void WGlRenderBackend::createTexture(TextureHandle handle, ETexture::Format format, const TextureDesc& desc, const char* name)
	{
		GlTexture& tex = m_textures[handle.textureIdx];

		GLenum eformat = toGlFormat(format);
		GLenum ecomponents = toGlComponents(desc.components);
		
		tex.m_format = eformat;
		tex.m_components = ecomponents;

		glGenTextures(1, &tex.m_id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(eformat, tex.m_id);

		if (desc.width % 2 == 0)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		}
		else
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
		}

		glTexParameterf(eformat, GL_TEXTURE_MAG_FILTER, toGlFilter(desc.magFilter));
		glTexParameterf(eformat, GL_TEXTURE_MIN_FILTER, toGlFilter(desc.minFilter));

		glTexParameteri(eformat, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
		glTexParameteri(eformat, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
		
		if (format > ETexture::Tex2D)
		{
			glTexParameterf(eformat, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}

		if (format == ETexture::CubeMap)
		{
			tex.m_cubeface = 0;
		}

		checkGlError();
	}

	GLenum toGlAttachment(ERenderAttachment::Type type)
	{
		static GLenum attachments[] = {GL_COLOR_ATTACHMENT0, GL_STENCIL_ATTACHMENT, GL_DEPTH_ATTACHMENT};
		return attachments[type];
	}

	void WGlRenderBackend::createRenderTarget(RenderTargetHandle handle, const RenderTargetDesc& desc)
	{
		GlFramebuffer& fb = m_framebuffers[handle.idx];
		fb.m_renderAttachments = desc.attachment;

		glCreateFramebuffers(1, &fb.m_id);

		if (desc.attachment & ERenderAttachment::Color)
		{
			glGenTextures(1, &fb.m_colorTex);
			glBindTexture(GL_TEXTURE_2D, fb.m_colorTex);
			glTexStorage2D(fb.m_colorTex, 1, GL_RGBA8, desc.width, desc.height);
			glNamedFramebufferTexture(fb.m_id, GL_COLOR_ATTACHMENT0, fb.m_colorTex, 0);
		}

		if (desc.attachment & ERenderAttachment::Stencil)
		{
			glGenTextures(1, &fb.m_stencilTex);
			glBindTexture(GL_TEXTURE_2D, fb.m_stencilTex);
			glTexStorage2D(fb.m_stencilTex, 1, GL_STENCIL_INDEX8, desc.width, desc.height);
			glNamedFramebufferTexture(fb.m_id, GL_STENCIL_ATTACHMENT, fb.m_stencilTex, 0);
		}

		if (desc.attachment & ERenderAttachment::Depth)
		{
			glGenTextures(1, &fb.m_depthTex);
			glBindTexture(GL_TEXTURE_2D, fb.m_depthTex);
			glTexStorage2D(fb.m_depthTex, 1, GL_DEPTH_COMPONENT32F, desc.width, desc.height);
			glNamedFramebufferTexture(fb.m_id, GL_DEPTH_ATTACHMENT, fb.m_depthTex, 0);
		}
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

	void WGlRenderBackend::setUniformImpl(const GlUniform& uniform, const void* data)
	{
		switch (uniform.m_type)
		{
		case EUniform::Mat4:
		{
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

	void WGlRenderBackend::setVertexBuffer(VertexBufferHandle vb)
	{
		GlVertexBuffer buffer = m_vertexBuffers[vb.idx];
		glBindVertexArray(buffer.m_id);
	}

	void WGlRenderBackend::setIndexBuffer(IndexBufferHandle ib)
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
			GL_TRIANGLES,
			GL_TRIANGLE_STRIP,
			GL_QUADS
		};

		return glType[type];
	}

	decltype(UniformHandle::idx) WGlRenderBackend::getUniform(ProgramHandle boundProgram, const UniformInfo& info) const
	{
		Hash name = info.nameHash;
		Hash hashWithProgram = hashBytes(&boundProgram.idx, sizeof(decltype(ProgramHandle::idx)), name);

		if (m_uniformMap.find(hashWithProgram) != m_uniformMap.end())
		{
			return m_uniformMap.at(hashWithProgram).idx;
		}
		else
		{
			return UniformHandle::invalidValue;
		}
	}

	void WGlRenderBackend::bindUniforms(ProgramHandle boundProgram, const UniformInfo* uniforms, size_t count)
	{
		// NOTE(Phil): At some point, the bound uniforms should be cached so that we don't constantly
		// recopy their data.

		for (size_t i = 0; i < count; ++i)
		{
			const UniformInfo& info = uniforms[i];
			auto uniformIdx = getUniform(boundProgram, info);

			if (UniformHandle::invalidValue == uniformIdx)
			{
				Logger::errorf("Uniform %s does not exist in program with handle id %d", info.name, boundProgram.idx);
				continue;
			}

			if (m_uniforms[uniformIdx].m_type == info.type)
			{
				setUniformImpl(m_uniforms[uniformIdx], info.data);
			}
			else
			{
				Logger::error("Uniform exists in program, but type is mismatched");
			}
		}

		checkGlError();
	}

	void WGlRenderBackend::bindTexture(const GlUniform& uniform, const GlTexture& texture)
	{
		assert(m_activeTextures < getMaxTextureUnits()); 
		glActiveTexture(GL_TEXTURE0 + m_activeTextures); 	
		glBindTexture(texture.m_format, texture.m_id); 
		glUniform1i(uniform.m_location, m_activeTextures);
	
		m_activeTextures++;
	}

	void WGlRenderBackend::bindTextures(ProgramHandle boundProgram, const UniformInfo* locations, const TextureHandle* handles, size_t count)
	{
		for (size_t i = 0; i < count; ++i)
		{
			const UniformInfo& info = locations[i];
			auto uniformIdx = getUniform(boundProgram, info);

			if (UniformHandle::invalidValue == uniformIdx)
			{
				Logger::errorf("Texture %s does not exist in program with handle id %d", info.name, boundProgram.idx);
				continue;
			}

			if (m_uniforms[uniformIdx].m_type == EUniform::Int)
			{
				bindTexture(m_uniforms[uniformIdx], m_textures[handles[i].textureIdx]);
			}
			else
			{
				Logger::error("Uniform exists in program, but type is not a sampler type");
			}
		}

		getGlErrorString();
		checkGlError();
	}

	void WGlRenderBackend::setState(const CStateGroup& state)
	{
		setProgram(state.program);
		setDepth(state.depth);
		bindUniforms(state.program, state.uniforms, state.uniformCount);
		bindTextures(state.program, state.textureLocations, state.textures, state.textureCount);
		//setBlend(state.blend);
		//setStencil(state.stencil);
		//setRaster(state.raster);
		m_activeTextures = 0;
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

	void WGlRenderBackend::clearRenderTarget(RenderTargetHandle handle, EBuffer::Type bitToClear, RGBA clearColor)
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