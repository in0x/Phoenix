#pragma once

#include "RIOpenGLResourceStore.hpp"
#include "RIDeviceOpenGL.hpp"

#include <Render/RIDefs.hpp>
#include <Core/Logger.hpp>

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
				static_cast<GLsizei>(data.m_size),
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
			Logger::error("Failed to create Fragment Shader!");
			m_resources->m_fragmentshaders.destroyResource(handle);
			handle.invalidate();
		}

		return handle;
	}

	bool checkIsSamplerType(GLenum typeVal)
	{
		return (typeVal == GL_TEXTURE_2D
			|| typeVal == GL_TEXTURE_3D
			|| typeVal == GL_TEXTURE_CUBE_MAP
			|| typeVal == GL_TEXTURE_1D);
	}

	void registerActiveUniforms(GlProgram& program)
	{
		GLint count = 0;
		GLint size = 0;
		GLenum type;

		GLint bufSize = 0;
		glGetProgramiv(program.m_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &bufSize);

		std::vector<GLchar> name(bufSize);
		GLsizei length = 0;

		glGetProgramiv(program.m_id, GL_ACTIVE_UNIFORMS, &count);

		for (GLint i = 0; i < count; ++i)
		{
			glGetActiveUniform(program.m_id, (GLuint)i, bufSize, &length, &size, &type, name.data());
			GLint location = glGetUniformLocation(program.m_id, name.data());
			program.m_activeUniforms.registerUniform(name.data(), program.m_id, location, size, type);
		}

		glGetProgramiv(program.m_id, GL_ACTIVE_UNIFORM_BLOCKS, &count);
	
		glGetProgramiv(program.m_id, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &bufSize);
		name.clear();
		name.resize(bufSize);
		
		GLint blockSize = 0;

		for (GLint i = 0; i < count; ++i)
		{
			glGetActiveUniformBlockName(program.m_id, i, bufSize, &length, name.data());
			glGetActiveUniformBlockiv(program.m_id, i, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

			program.m_activeUniforms.registerUniform(name.data(), program.m_id, i, blockSize, GL_UNIFORM_BUFFER); 
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
		registerActiveUniforms(*program);

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
		case EPixelFormat::RG8:
			return GL_RG8; 
		case EPixelFormat::R8:
			return GL_R8;
		case EPixelFormat::SRGB8:
			return GL_SRGB8;
		case EPixelFormat::SRGBA8:
			return GL_SRGB8_ALPHA8;
		case EPixelFormat::RGB16F:
			return GL_RGB16F;
		case EPixelFormat::RGBA16F:
			return GL_RGBA16F;
		case EPixelFormat::RGB32F:
			return GL_RGB32F;
		case EPixelFormat::RGBA32F:
			return GL_RGBA32F;
		case EPixelFormat::Depth32F:
			return GL_DEPTH_COMPONENT32F;
		case EPixelFormat::Depth16I:
			return GL_DEPTH_COMPONENT16;
		case EPixelFormat::Stencil8I:
			return GL_STENCIL_INDEX8;

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
		case EPixelFormat::RGBA16F:
		case EPixelFormat::RGBA32F:
		case EPixelFormat::SRGBA8:
			return GL_RGBA;
		case EPixelFormat::R8G8B8:
		case EPixelFormat::RGB16F:
		case EPixelFormat::RGB32F:
		case EPixelFormat::SRGB8:
			return GL_RGB;
		case EPixelFormat::RG8:
			return GL_RG;
		case EPixelFormat::R8:
			return GL_RED;
		case EPixelFormat::Depth32F:
			return GL_DEPTH_COMPONENT;
		case EPixelFormat::Depth16I:
			return GL_DEPTH_COMPONENT;
		case EPixelFormat::Stencil8I:
			return GL_STENCIL_INDEX;
		default:
			assert(false);
			return GL_INVALID_ENUM;
		}
	}

	GLenum toGlTexDatatype(EPixelFormat format)
	{
		switch (format)
		{
		case EPixelFormat::R8G8B8A8:
		case EPixelFormat::SRGBA8:
		case EPixelFormat::R8G8B8:
		case EPixelFormat::SRGB8:
		case EPixelFormat::RG8:
		case EPixelFormat::R8:
			return GL_UNSIGNED_BYTE;
		case EPixelFormat::Depth32F:
		case EPixelFormat::RGBA16F:
		case EPixelFormat::RGBA32F:
		case EPixelFormat::RGB16F:
		case EPixelFormat::RGB32F:
			return GL_FLOAT;
		case EPixelFormat::Depth16I:
		case EPixelFormat::Stencil8I:
			return GL_UNSIGNED_SHORT;
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

	GLenum toGlMipFilter(ETextureFilter minFilter, ETextureFilter mipFilter)
	{
		if (minFilter == ETextureFilter::Nearest && mipFilter == ETextureFilter::Nearest)
		{
			return GL_NEAREST_MIPMAP_NEAREST;
		}
		else if (minFilter == ETextureFilter::Linear && mipFilter == ETextureFilter::Nearest)
		{
			return GL_LINEAR_MIPMAP_NEAREST;
		}
		else if (minFilter == ETextureFilter::Nearest && mipFilter == ETextureFilter::Linear)
		{
			return GL_NEAREST_MIPMAP_LINEAR;
		}
		else if (minFilter == ETextureFilter::Linear && mipFilter == ETextureFilter::Linear)
		{ 
			return GL_LINEAR_MIPMAP_LINEAR;
		}
		else
		{
			return GL_NEAREST;
		}
	}

	GLenum toGlWrap(ETextureWrap wrap)
	{
		switch (wrap)
		{
		case ETextureWrap::ClampToEdge:
			return GL_CLAMP_TO_EDGE;
		case ETextureWrap::Repeat:
			return GL_REPEAT;
		case ETextureWrap::MirroredRepeat:
			return GL_MIRRORED_REPEAT;
		default:
			assert(false);
			return GL_INVALID_ENUM;
		}
	}

	void createTextureBase(RITexture& texture, GlTextureBase& glTex, const TextureDesc& desc, GLenum textureType)
	{
		glTex.m_components = toGlComponents(desc.pixelFormat);
		glTex.m_dataType = toGlTexDatatype(desc.pixelFormat);
		glTex.m_pixelFormat = toGlFormat(desc.pixelFormat);
		texture.m_numMips = desc.numMips;
		texture.m_numMips = desc.numMips;

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

		GLenum magFilter = toGlFilter(desc.magFilter);

		GLenum minFilter = 0;
		if (desc.numMips > 0)
		{
			minFilter = toGlMipFilter(desc.minFilter, desc.mipFilter);
		}
		else
		{
			minFilter = toGlFilter(desc.minFilter);
		}

		glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, magFilter);
		glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, minFilter);

		glTexParameteri(textureType, GL_TEXTURE_WRAP_S, toGlWrap(desc.wrapU));
		glTexParameteri(textureType, GL_TEXTURE_WRAP_T, toGlWrap(desc.wrapV));
		glTexParameteri(textureType, GL_TEXTURE_WRAP_R, toGlWrap(desc.wrapW));
	}

	Texture2DHandle	RIDeviceOpenGL::createTexture2D(const TextureDesc& desc)
	{
		Texture2DHandle handle = m_resources->m_texture2Ds.allocateResource();
		GlTexture2D* texture = m_resources->m_texture2Ds.getResource(handle);

		createTextureBase(*texture, texture->m_glTex, desc, GL_TEXTURE_2D);

		texture->m_width = desc.width;
		texture->m_height = desc.height;

		glTexStorage2D(GL_TEXTURE_2D, desc.numMips + 1, texture->m_glTex.m_pixelFormat, desc.width, desc.height);

		if (checkGlErrorOccured())
		{
			Logger::error("Failed to create Texture2D!");
			m_resources->m_texture2Ds.destroyResource(handle);
			handle.invalidate();
		}

		// NOTE(Phil): May want to add option for rendertarget-only textures later.

		return handle;
	}

	TextureCubeHandle RIDeviceOpenGL::createTextureCube(const TextureDesc& desc)
	{
		TextureCubeHandle handle = m_resources->m_textureCubes.allocateResource();
		GlTextureCube* texture = m_resources->m_textureCubes.getResource(handle);

		createTextureBase(*texture, texture->m_glTex, desc, GL_TEXTURE_CUBE_MAP);

		assert(desc.width == desc.height);
		texture->m_size = desc.width;

		glTexStorage2D(GL_TEXTURE_CUBE_MAP, desc.numMips + 1, texture->m_glTex.m_pixelFormat, texture->m_size, texture->m_size);

		if (checkGlErrorOccured())
		{
			Logger::error("Failed to create TextureCube!");
			m_resources->m_textureCubes.destroyResource(handle);
			handle.invalidate();
		}

		return handle;
	}

	UniformHandle RIDeviceOpenGL::createUniform(const char* name, EUniformType type)
	{
		UniformHandle handle = m_resources->m_uniforms.allocateResource();
		RIUniform* uniform = m_resources->m_uniforms.getResource(handle);
		uniform->m_type = type;
		uniform->m_nameHash = HashFNV<const char*>()(name);
		strncpy(uniform->m_debugName, name, RIUniform::MAX_NAME_LENGTH);
		return handle;
	}

	GLenum toGlAttachment(RenderTargetDesc::EAttachment attachment)
	{
		if (attachment < RenderTargetDesc::NumMaxColors && attachment != RenderTargetDesc::NumMaxColors)
		{
			return GL_COLOR_ATTACHMENT0 + static_cast<int32_t>(attachment);
		}

		switch (attachment)
		{
		case RenderTargetDesc::Depth:
			return GL_DEPTH_ATTACHMENT;
		case RenderTargetDesc::Stencil:
			return GL_STENCIL_ATTACHMENT;
		case RenderTargetDesc::DepthStencil:
			return GL_DEPTH_STENCIL_ATTACHMENT;
		default:
			assert(false);
			return GL_INVALID_ENUM;
		}
	}

	RenderTargetHandle RIDeviceOpenGL::createRenderTarget(const RenderTargetDesc& desc)
	{
		RenderTargetHandle handle = m_resources->m_framebuffers.allocateResource();
		GlFramebuffer* framebuffer = m_resources->m_framebuffers.getResource(handle);

		glGenFramebuffers(1, &framebuffer->m_id);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->m_id);

		bool bHasColor = attachColors(desc, framebuffer);
		assert(bHasColor);

		bool bHasDepthStencil = attachifValid(desc.depthStencilAttach, framebuffer, RenderTargetDesc::DepthStencil);

		if (!bHasDepthStencil)
		{
			attachifValid(desc.depthAttach, framebuffer, RenderTargetDesc::Depth);
			attachifValid(desc.stencilAttach, framebuffer, RenderTargetDesc::Stencil);
		}

		glDrawBuffers(framebuffer->m_colorAttachCount, framebuffer->m_colorAttachments);

		GLenum framebufferstate = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		bool bNotComplete = GL_FRAMEBUFFER_COMPLETE != framebufferstate;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (checkGlErrorOccured() || bNotComplete)
		{
			Logger::error("Failed to create RenderTarget!");
			m_resources->m_framebuffers.destroyResource(handle);
			handle.invalidate();
		}

		return handle;
	}

	// Returns false if no color attachments exist.
	bool RIDeviceOpenGL::attachColors(const RenderTargetDesc& desc, GlFramebuffer* fb)
	{
		bool bHasAnyColors = false;

		for (size_t i = 0; i < RenderTargetDesc::NumMaxColors; ++i)
		{
			Texture2DHandle texHandle = desc.colorAttachs[i];

			if (texHandle.isValid())
			{
				GlTexture2D* texture = m_resources->m_texture2Ds.getResource(texHandle);
				fb->m_attachedTextures[i] = texture;

				GLenum attachNum = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i);
				glFramebufferTexture(GL_FRAMEBUFFER, attachNum, texture->m_glTex.m_id, 0);
				fb->m_colorAttachments[i] = attachNum;
			
				bHasAnyColors = true;
				fb->m_colorAttachCount++;
			}
		}

		return bHasAnyColors;
	}

	bool RIDeviceOpenGL::attachifValid(Texture2DHandle tex, GlFramebuffer* fb, RenderTargetDesc::EAttachment attachment)
	{
		if (!tex.isValid())
		{
			return false;
		}

		const GlTexture2D* texture = m_resources->m_texture2Ds.getResource(tex);
		fb->m_attachedTextures[attachment] = texture;
		glFramebufferTexture(GL_FRAMEBUFFER, toGlAttachment(attachment), texture->m_glTex.m_id, 0);
		return true;
	}

	size_t cbTypeToSize(ECBType type)
	{
		// These sizes probably dont work by themselves since we need to factor in padding
		switch (type)
		{
		case ECBType::Vec4:
			return 32 * 4;
		case ECBType::Mat4:
			return 32 * 4 * 4;
		case ECBType::Int:
			return 32;
		case ECBType::Vec3:
			return 32 * 3;
		case ECBType::Mat3:
			return 32 * 3 * 3;
		case ECBType::Float:
			return 32;
		default:
			assert(false);
			return 0;
		}
	}
	
	ConstantBufferHandle RIDeviceOpenGL::createConstantBuffer(const ConstantBufferDesc& desc)
	{
		ConstantBufferHandle handle = m_resources->m_constantBuffers.allocateResource();
		GlConstantBuffer* cb = m_resources->m_constantBuffers.getResource(handle);

		cb->m_desc = desc;

		size_t bufferSizeBytes = 0;

		for (size_t i = 0; i < desc.numMembers; ++i)
		{
			const CBMember& member = desc.members[i];

			bufferSizeBytes += cbTypeToSize(member.type) * member.arrayLen;
		}

		bufferSizeBytes *= desc.arrayLen;	
		
		glGenBuffers(1, &cb->m_id);
		glBindBuffer(GL_UNIFORM_BUFFER, cb->m_id);
		glBufferData(GL_UNIFORM_BUFFER,	bufferSizeBytes, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		if (checkGlErrorOccured())
		{
			Logger::error("An error occured during ConstantBuffer creation.");
			m_resources->m_constantBuffers.destroyResource(handle);
			handle.invalidate();
		}

		return handle;
	}
}