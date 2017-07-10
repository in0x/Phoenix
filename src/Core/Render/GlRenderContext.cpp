#include "GlRenderContext.hpp"
#include "../Logger.hpp"

namespace Phoenix
{
	void GlRenderContext::init()
	{
		
	}

	VertexBufferHandle GlRenderContext::createVertexBuffer()
	{
		VertexBufferHandle handle;
		VertexBufferHandle::isValid(handle);
		return handle;
	}

	 IndexBufferHandle GlRenderContext::createIndexBuffer() { return IndexBufferHandle(); };
	 ShaderHandle GlRenderContext::createShader() override { return ShaderHandle(); };
	 ProgramHandle GlRenderContext::createProgram() override { return ProgramHandle(); };
	 TextureHandle GlRenderContext::createTexture() override { assert(false);  return TextureHandle(); };
	 FrameBufferHandle GlRenderContext::createFrameBuffer() override { assert(false); return FrameBufferHandle(); };

private:
	std::vector<VertexBufferHandle> m_vertexBuffers;
	std::vector<IndexBufferHandle> m_indexBuffers;
	std::vector<ShaderHandle> m_shaders;
	std::vector<ProgramHandle> m_programs;
}; }