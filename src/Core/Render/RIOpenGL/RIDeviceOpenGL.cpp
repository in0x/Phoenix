#pragma once

#include "RIOpenGLResourceStore.hpp"
#include "RIDeviceOpenGL.hpp"

#include "../RenderDefinitions.hpp"
#include "../../Logger.hpp"

#include <assert.h>
#include <vector>

namespace Phoenix
{
	RIDeviceOpenGL::RIDeviceOpenGL(RIOpenGLResourceStore* resources)
		: m_resources(resources)
	{
	}

	RIDeviceOpenGL::~RIDeviceOpenGL()
	{
	}

	GLuint createVBO(size_t typeSize, size_t elementCount, const void* data)
	{
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, typeSize * elementCount, data, GL_STATIC_DRAW); // TODO(Phil): Implement GL_DYNAMIC_DRAW																			  
		
		return vbo;
	}
		
	GLuint toGlAttribType(EAttributeType type)
	{
		switch (type)
		{
		case EAttributeType::Float:
			return GL_FLOAT;
		case EAttributeType::Double:
			return GL_DOUBLE;
		case EAttributeType::Uint:
			return GL_UNSIGNED_INT;
		case EAttributeType::Int:
			return GL_INT;

		case EAttributeType::Count:
		default:
			assert(false);
			return GL_INVALID_ENUM;
		}
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
				toGlAttribType(decl.m_type),
				data.m_bNormalize,
				data.m_size,
				nullptr);
		}
		
		if (checkGlErrorOccured())
		{
			Logger::error("An error occured during VertexBuffer creation.");
			m_resources->m_vertexbuffers.destroyResource(handle);
			handle.invalidate();
		}

		return handle;
	}

	IndexBufferHandle RIDeviceOpenGL::createIndexBuffer(size_t elementSizeBytes, size_t count, const void* data) 
	{
		IndexBufferHandle handle = m_resources->m_indexbuffers.allocateResource();
		GlIndexBuffer* buffer = m_resources->m_indexbuffers.getResource(handle);
		buffer->m_numElements = count;

		glGenBuffers(1, &buffer->m_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->m_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementSizeBytes * count, data, GL_STATIC_DRAW); // TODO(Phil): Add dynamic support

		if (checkGlErrorOccured())
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

		checkGlErrorOccured();
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
	
	void registerActiveUniforms(const GlProgram& program, GlExisitingUniforms& uniformstore)
	{
		GLint count;
		GLint size;
		GLenum type;
	
		GLint bufSize = 0;
		glGetProgramiv(program.m_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &bufSize);

		if (0 == bufSize)
		{
			return;
		}

		std::vector<GLchar> name(bufSize);
		GLsizei length;
	
		glGetProgramiv(program.m_id, GL_ACTIVE_UNIFORMS, &count);
	
		for (GLint i = 0; i < count; ++i)
		{
			glGetActiveUniform(program.m_id, (GLuint)i, bufSize, &length, &size, &type, name.data());
			uniformstore.registerUniform(name.data(), program.m_id, i, size);
		}
	}

	ProgramHandle RIDeviceOpenGL::createProgram(VertexShaderHandle vsHandle, FragmentShaderHandle fsHandle) 
	{
		ProgramHandle handle = m_resources->m_programs.allocateResource();

		GlProgram* program = m_resources->m_programs.getResource(handle);
		GlVertexShader* vertexshader = m_resources->m_vertexshaders.getResource(vsHandle);
		GlFragmentShader* fragmentshader = m_resources->m_fragmentshaders.getResource(fsHandle);

		GLuint programId = glCreateProgram();

		glAttachShader(programId, vertexshader->m_glShader.m_id);
		glAttachShader(programId, fragmentshader->m_glShader.m_id);

		glLinkProgram(programId);

		if (!glIsProgram(programId))
		{
			Logger::error("Failed to compile shader program.");
			m_resources->m_programs.destroyResource(handle);
			handle.invalidate();
			return handle;
		}

		program->m_id = programId;
		registerActiveUniforms(*program, m_resources->m_actualUniforms);

		if (checkGlErrorOccured())
		{
			Logger::error("Failed to compile Shader program!");
			m_resources->m_programs.destroyResource(handle);
			handle.invalidate();
		}

		return handle;
	}

	GLenum toGlFormat(EPixelFormat format)
	{
		switch (format)
		{
		case EPixelFormat::R8G8B8A8:
			return GL_RGBA8;
		case EPixelFormat::R8G8B8:
			return GL_RGB8;
		default:
			assert(false);
			return GL_INVALID_ENUM;
		}
	}

	GLint toGlComponents(EPixelFormat format)
	{
		switch (format)
		{
		case EPixelFormat::R8G8B8A8:
			return GL_RGBA;
		case EPixelFormat::R8G8B8:
			return GL_RGB;
		default:
			assert(false);
			return GL_INVALID_ENUM;
		}
	}

	GLenum toGlFilter(ETextureFilter filter)
	{
		switch (filter)
		{
		case ETextureFilter::Linear:
				return GL_LINEAR;
		case ETextureFilter::Nearest:
			return GL_NEAREST;
		default:
			assert(false);
			return GL_INVALID_ENUM;
		}
	}

	void createTextureBase(RITexture& texture, GlTextureBase& glTex, const TextureDesc& desc, const char* name, GLenum textureType)
	{
		glTex.m_pixelFormat = toGlFormat(desc.pixelFormat);
		glTex.m_components = toGlComponents(desc.pixelFormat);
		glTex.m_dataType = GL_UNSIGNED_BYTE; // NOTE(Phil): This might be problematic with higher pixeldepth textures later (hdr)
		texture.m_numMips = desc.numMips;
		texture.m_namehash = HashFNV<const char*>()(name);
		
		glGenTextures(1, &glTex.m_id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(textureType, glTex.m_id);

		if (desc.width % 2 == 0)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		}
		else
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		}
	}

	Texture2DHandle	RIDeviceOpenGL::createTexture2D(const TextureDesc& desc, const char* name) 
	{ 
		Texture2DHandle handle = m_resources->m_texture2Ds.allocateResource();
		GlTexture2D* texture = m_resources->m_texture2Ds.getResource(handle);

		createTextureBase(*texture, texture->m_glTex, desc, name, GL_TEXTURE_2D);

		texture->m_width = desc.width;
		texture->m_height = desc.height;

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGlFilter(desc.magFilter));
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGlFilter(desc.minFilter));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		if (checkGlErrorOccured())
		{
			Logger::error("Failed to create Texture2D!");
			m_resources->m_texture2Ds.destroyResource(handle);
			handle.invalidate();
		}

		return handle;
	}
	
	TextureCubeHandle RIDeviceOpenGL::createTextureCube(const TextureDesc& desc, const char* name)
	{
		TextureCubeHandle handle = m_resources->m_textureCubes.allocateResource();
		GlTextureCube* texture = m_resources->m_textureCubes.getResource(handle);

		createTextureBase(*texture, texture->m_glTex, desc, name, GL_TEXTURE_CUBE_MAP);

		assert(desc.width == desc.height);
		texture->m_size = desc.width;

		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, toGlFilter(desc.magFilter));
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, toGlFilter(desc.minFilter));

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		if (checkGlErrorOccured())
		{
			Logger::error("Failed to create TextureCube!");
			m_resources->m_textureCubes.destroyResource(handle);
			handle.invalidate();
		}

		return handle;
	}

	//if (format > ETexture::Tex2D) // Relevant for Texture3D
	//{
	//glTexParameterf(format, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//}

	UniformHandle RIDeviceOpenGL::createUniform(const char* name, EUniformType type)
	{
		UniformHandle handle = m_resources->m_uniforms.allocateResource();
		RIUniform* uniform = m_resources->m_uniforms.getResource(handle);
		uniform->m_type = type;
		uniform->m_nameHash = HashFNV<const char*>()(name);
		return handle;
	}

	RenderTargetHandle RIDeviceOpenGL::createRenderTarget(const RenderTargetDesc& desc) { assert(false); return RenderTargetHandle{}; }
}