#pragma once

#include "../RIResourceContainer.hpp"
#include "../RIResourceHandles.h"

#include "RIResourcesOpenGL.hpp"
#include "RIGlExistingUniforms.hpp"

namespace Phoenix
{
	struct RIOpenGLResourceStore
	{
		RIResourceContainer<GlVertexBuffer,   VertexBufferHandle,   1024> m_vertexbuffers;
		RIResourceContainer<GlIndexBuffer,	  IndexBufferHandle,    1024> m_indexbuffers;
		RIResourceContainer<GlVertexShader,	  VertexShaderHandle,   256 > m_vertexshaders;
		RIResourceContainer<GlFragmentShader, FragmentShaderHandle, 256 > m_fragmentshaders;
		RIResourceContainer<GlProgram,		  ProgramHandle,		256 > m_programs;
		//RIResourceContainer<RIUniformInt,	  IntUniformHandle,		128 > m_intUniforms;
		//RIResourceContainer<RIUniformFloat,	  FloatUniformHandle,	128 > m_floatUniforms;
		//RIResourceContainer<RIUniformVec3,	  Vec3UniformHandle,	128 > m_vec3Uniforms;
		//RIResourceContainer<RIUniformVec4,	  Vec4UniformHandle,	128 > m_vec4Uniforms;
		//RIResourceContainer<RIUniformMat3,	  Mat3UniformHandle,	128 > m_mat3Uniforms;
		//RIResourceContainer<RIUniformMat4,	  Mat4UniformHandle,	128 > m_mat4Uniforms;
		RIResourceContainer<GlTexture2D,	  Texture2DHandle,		1024> m_texture2Ds;
		RIResourceContainer<GlFramebuffer,	  RenderTargetHandle,   256 > m_framebuffer;
		GlExisitingUniforms m_actualUniforms;
	};
}