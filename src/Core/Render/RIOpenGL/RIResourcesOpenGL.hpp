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

	//class GlUniformInt : public RIUniformInt
	//{
	//public:
	//	GlUniform m_glUniform;
	//};

	//class GlUniformFloat : public RIUniformFloat
	//{
	//public:
	//	GlUniform m_glUniform;
	//};

	//class GlUniformVec3 : public RIUniformVec3
	//{
	//public:
	//	GlUniform m_glUniform;
	//};

	//class GlUniformVec4 : public RIUniformVec4
	//{
	//public:
	//	GlUniform m_glUniform;
	//};

	//class GlUniformMat3 : public RIUniformMat3
	//{
	//public:
	//	GlUniform m_glUniform;
	//};

	//class GlUniformMat4 : public RIUniformMat4
	//{
	//public:
	//	GlUniform m_glUniform;
	//};

	class GlTexture2D : public RITexture2D
	{
	public:
		GLuint m_id;
		GLenum m_format;
		GLenum m_components;
		GLenum m_dataType;
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