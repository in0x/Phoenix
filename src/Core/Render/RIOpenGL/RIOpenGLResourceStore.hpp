#pragma once

#include "../RIResourceContainer.hpp"
#include "../RIResourceHandles.h"
#include "RIResourcesOpenGL.hpp"
#include <unordered_map>

namespace Phoenix
{
	class GlExisitingUniforms
	{
	public:
		void registerUniform(const char* name, GLuint programID, GLuint location);

		bool getUniformIfExisting(Hash hash, GlUniform& outUniform);

	private:
		std::unordered_map<Hash, GlUniform> m_existingUniforms;
	};

	struct RIOpenGLResourceStore
	{
		RIResourceContainer<GlVertexBuffer,   VertexBufferHandle,   1024> m_vertexbuffers;
		RIResourceContainer<GlIndexBuffer,	  IndexBufferHandle,    1024> m_indexbuffers;
		RIResourceContainer<GlVertexShader,	  VertexShaderHandle,   256 > m_vertexshaders;
		RIResourceContainer<GlFragmentShader, FragmentShaderHandle, 256 > m_fragmentshaders;
		RIResourceContainer<GlProgram,		  ProgramHandle,		256 > m_programs;
		RIResourceContainer<GlUniformInt,	  IntUniformHandle,		128 > m_intUniforms;
		RIResourceContainer<GlUniformFloat,	  FloatUniformHandle,	128 > m_floatUniforms;
		RIResourceContainer<GlUniformVec3,	  Vec3UniformHandle,	128 > m_vec3Uniforms;
		RIResourceContainer<GlUniformVec4,	  Vec4UniformHandle,	128 > m_vec4Uniforms;
		RIResourceContainer<GlUniformMat3,    Mat3UniformHandle,	128 > m_mat3Uniforms;
		RIResourceContainer<GlUniformMat4,    Mat4UniformHandle,	128 > m_mat4Uniforms;
		RIResourceContainer<GlTexture2D,	  Texture2DHandle,		1024> m_texture2Ds;
		RIResourceContainer<GlFramebuffer,	  RenderTargetHandle,   256 > m_framebuffer;
	};
}