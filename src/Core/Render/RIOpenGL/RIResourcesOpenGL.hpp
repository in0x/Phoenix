#pragma once

#include "../RIResources.hpp"
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
		GLint m_numElements;
	};

	class GlTexture2D : public RITexture2D
	{
	public:
		GLuint m_id;
		GLenum m_pixelFormat;
		GLenum m_components;
		GLenum m_dataType;
	};

	class GlTextureCube : public RITextureCube
	{
		int8_t m_cubeface;
	};

	class GlFramebuffer : public RIRenderTarget
	{
	public:
		GLuint m_id;
		GLuint m_colorTex;
		GLuint m_stencilTex;
		GLuint m_depthTex;
		int32_t m_renderAttachments;
	};
}