#pragma once

#include <Render/RIResourceContainer.hpp>
#include <Render/RIResourceHandles.hpp>

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
		RIResourceContainer<GlTexture2D,	  Texture2DHandle,		4096> m_texture2Ds;
		RIResourceContainer<GlFramebuffer,	  RenderTargetHandle,   256 > m_framebuffers;
		RIResourceContainer<RIUniform,		  UniformHandle,		2048> m_uniforms;
		RIResourceContainer<GlTextureCube,	  TextureCubeHandle,	256>  m_textureCubes;
		RIResourceContainer<GlConstantBuffer, ConstantBufferHandle, 1024> m_constantBuffers;
	};
}