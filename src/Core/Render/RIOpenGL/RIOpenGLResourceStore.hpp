#pragma once

#include "../RIResourceContainer.hpp"
#include "../RIResourceHandles.hpp"

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
		RIResourceContainer<GlTexture2D,	  Texture2DHandle,		1024> m_texture2Ds;
		RIResourceContainer<GlFramebuffer,	  RenderTargetHandle,   256 > m_framebuffers;
		RIResourceContainer<RIUniform,		  UniformHandle,		512>  m_uniforms;
		RIResourceContainer<GlTextureCube,	  TextureCubeHandle,	256>  m_textureCubes;
		GlExisitingUniforms m_actualUniforms;
	};
}