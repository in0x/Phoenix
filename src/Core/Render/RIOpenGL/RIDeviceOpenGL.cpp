#pragma once

#include "RIOpenGLResourceStore.hpp"
#include "RIDeviceOpenGL.hpp"

#include "../../Logger.hpp"

#include <assert.h>

namespace Phoenix
{
	RIDeviceOpenGL::RIDeviceOpenGL(RIOpenGLResourceStore* resources)
		: m_resources(resources)
	{
	}

	RIDeviceOpenGL::~RIDeviceOpenGL()
	{
	}

	GLuint createVBO(size_t typeSize, uint32_t elementCount, const void* data)
	{
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, typeSize * elementCount, data, GL_STATIC_DRAW); // TODO(Phil): Implement GL_DYNAMIC_DRAW																			  
		
		return vbo;
	}
		
	GLuint attribSizeToGl(EAttributeType::Value type)
	{
		static constexpr GLuint glType[EAttributeType::Count] =
		{
			GL_DOUBLE,
			GL_FLOAT,
			GL_UNSIGNED_INT,
			GL_INT
		};
		
		return glType[type];
	}
	
	VertexBufferHandle RIDeviceOpenGL::createVertexBuffer(const VertexBufferFormat& format) 
	{
		VertexBufferHandle handle = m_resources->m_vertexbuffers.allocateResource();
		GlVertexBuffer* buffer = m_resources->m_vertexbuffers.getResource(handle);

		glGenVertexArrays(1, &buffer->m_id);
		glBindVertexArray(buffer->m_id);

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
		
		if (checkGlError())
		{
			Logger::error("An error occured during VertexBuffer creation.");
			m_resources->m_vertexbuffers.destroyResource(handle);
			handle.invalidate();
		}

		return handle;
	}

	IndexBufferHandle RIDeviceOpenGL::createIndexBuffer(size_t size, uint32_t count, const void* data) 
	{
		IndexBufferHandle handle = m_resources->m_indexbuffers.allocateResource();
		GlIndexBuffer* buffer = m_resources->m_indexbuffers.getResource(handle);

		glGenBuffers(1, &buffer->m_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->m_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * count, data, GL_STATIC_DRAW); // TODO(Phil): Add dynamic support

		if (checkGlError())
		{
			Logger::error("An error occured during IndexBuffer creation.");
			m_resources->m_indexbuffers.destroyResource(handle);
			handle.invalidate();
		}

		return handle;
	};
	
	// Returns true if an error occured during shader compilation.
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
				return false;
			}
	
			delete[] infoLog;
		}
	
		else
		{
			Logger::errorf("ID %d is not a shader", shader);
			return false;
		}
	
		return true;
	}

	bool createShader(GlShader* shader, GLenum shaderType, const char* source)
	{
		if (source == nullptr)
		{
			return false;
		}

		shader->m_shaderType = shaderType;
		shader->m_id = glCreateShader(shaderType);
		
		glShaderSource(shader->m_id, 1, (const char**)&source, NULL);
		glCompileShader(shader->m_id);

		checkGlError();
		return getAndLogShaderLog(shader->m_id);
	}
	
	VertexShaderHandle RIDeviceOpenGL::createVertexShader(const char* source) 
	{
		VertexShaderHandle handle = m_resources->m_vertexshaders.allocateResource();		
		GlVertexShader* shader = m_resources->m_vertexshaders.getResource(handle);

		bool bSuccess = createShader(&shader->m_glShader, GL_VERTEX_SHADER, source);
	
		if (!bSuccess)
		{
			Logger::error("Failed to create Vertex Shader!");
			m_resources->m_vertexshaders.destroyResource(handle);
			handle.invalidate();
		}

		return handle;
	}

	FragmentShaderHandle RIDeviceOpenGL::createFragmentShader(const char* source) 
	{ 
		FragmentShaderHandle handle = m_resources->m_fragmentshaders.allocateResource();
		GlFragmentShader* shader = m_resources->m_fragmentshaders.getResource(handle);
		
		bool bSuccess = createShader(&shader->m_glShader, GL_FRAGMENT_SHADER, source);

		if (!bSuccess)
		{
			Logger::error("Failed to create Vertex Shader!");
			m_resources->m_fragmentshaders.destroyResource(handle);
			handle.invalidate();
		}

		return handle;
	}
	
	ProgramHandle RIDeviceOpenGL::createProgram(VertexShaderHandle vsHandle, FragmentShaderHandle fsHandle) 
	{
		ProgramHandle handle = m_resources->m_programs.allocateResource();

		GlProgram* program = m_resources->m_programs.getResource(handle);
		GlVertexShader* vertexshader = m_resources->m_vertexshaders.getResource(vsHandle);
		GlFragmentShader* fragmentshader = m_resources->m_fragmentshaders.getResource(fsHandle);

		GLuint progHandle = glCreateProgram();

		glAttachShader(progHandle, vertexshader->m_glShader.m_id);
		glAttachShader(progHandle, fragmentshader->m_glShader.m_id);

		glLinkProgram(progHandle);

		if (!glIsProgram(progHandle))
		{
			Logger::error("Failed to compile shader program.");
			handle.invalidate();
			return handle;
		}

		program->m_id = progHandle;
		//registerActiveUniforms(handle);

		checkGlError();
	}

	IntUniformHandle	 RIDeviceOpenGL::createIntUniform(const char* name, int32_t value) { assert(false); return IntUniformHandle{}; }
	FloatUniformHandle   RIDeviceOpenGL::createFloatUniform(const char* name, float value) { assert(false); return FloatUniformHandle{}; }
	Vec3UniformHandle	 RIDeviceOpenGL::createVec3Uniform(const char* name, const Vec3& value) { assert(false); return Vec3UniformHandle{}; }
	Vec4UniformHandle	 RIDeviceOpenGL::createVec4Uniform(const char* name, const Vec4& value) { assert(false); return Vec4UniformHandle{}; }
	Mat3UniformHandle	 RIDeviceOpenGL::createMat3Uniform(const char* name, const Matrix3& value) { assert(false); return Mat3UniformHandle{}; }
	Mat4UniformHandle	 RIDeviceOpenGL::createMat4Uniform(const char* name, const Matrix4& value) { assert(false); return Mat4UniformHandle{}; }
	Texture2DHandle		 RIDeviceOpenGL::createTexture2D(const TextureDesc& desc) { assert(false); return Texture2DHandle{}; }
	RenderTargetHandle	 RIDeviceOpenGL::createRenderTarget(const RenderTargetDesc& desc) { assert(false); return RenderTargetHandle{}; }

}