#pragma once
#include "IRenderContext.hpp"
#include <vector>

namespace Phoenix
{
	class GlRenderContext : public IRenderContext
	{
	public:
		virtual void init() override;

		virtual VertexBufferHandle createVertexBuffer() override;
		virtual IndexBufferHandle createIndexBuffer()override;
		virtual ShaderHandle createShader() override;
		virtual ProgramHandle createProgram() override;
		virtual TextureHandle createTexture() override;
		virtual FrameBufferHandle createFrameBuffer() override;

	private:
		std::vector<VertexBufferHandle> m_vertexBuffers; // These vectors need replacing when I create custom allocators
		std::vector<IndexBufferHandle> m_indexBuffers;
		std::vector<ShaderHandle> m_shaders;
		std::vector<ProgramHandle> m_programs;
	};
}