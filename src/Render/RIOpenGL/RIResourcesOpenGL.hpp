#pragma once

#include <Render/RIResources.hpp>
#include "OpenGL.hpp"

namespace Phoenix
{
	class GlVertexBuffer : public RIVertexBuffer
	{
	public:
		GLuint m_id;
	};

	class GlIndexBuffer : public RIIndexBuffer
	{
	public:
		GLuint m_id;
	};

	struct GlShader
	{
		GLuint m_id;
		GLenum m_shaderType;
	};

	class GlVertexShader : public RIVertexShader
	{
	public:
		GlShader m_glShader;
	};

	class GlFragmentShader : public RIFragmentShader
	{
	public:
		GlShader m_glShader;
	};

	class GlProgram : public RIProgram
	{
	public:
		GLuint m_id;
	};

	struct GlUniform
	{
		GLuint m_location;
		GLint m_numElements; // An optional argument that carries the sampler type would be nice
		GLenum m_glType;
	};

	struct GlTextureBase
	{
		GLuint m_id;
		GLenum m_pixelFormat;
		GLenum m_components;
		GLenum m_dataType;
	};

	class GlTexture2D : public RITexture2D
	{
	public:
		GlTextureBase m_glTex;
	};

	class GlTextureCube : public RITextureCube
	{
	public:
		GlTextureBase m_glTex;
	};

	class GlFramebuffer : public RIRenderTarget
	{
	public:
		GlFramebuffer()
			: m_id(0)
		{}

		GLuint m_id;
		GlTexture2D* m_attachments[4];
	};
}