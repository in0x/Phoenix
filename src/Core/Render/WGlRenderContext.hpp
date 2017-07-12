#pragma once

#include <stdint.h>
#include <vector>
#include "Windows.h"
#include "IRenderContext.hpp"
#include "../OpenGL.hpp"

namespace Phoenix
{
	HANDLE(VertexBufferHandle, uint16_t)
	HANDLE(IndexBufferHandle, uint16_t)
	HANDLE(ShaderHandle, uint16_t)
	HANDLE(ProgramHandle, uint16_t)
	HANDLE(TextureHandle, uint16_t)
	HANDLE(FrameBufferHandle, uint16_t)

	struct GlVertexBuffer
	{
		GLuint m_id;
		GLenum m_target;
		uint32_t m_size;
		/*VertexDeclHandle m_decl;
		VaoCacheRef m_vcref;*/
	};

	class WGlRenderContext : public IRenderContext
	{
	public:
		explicit WGlRenderContext(HWND owningWindow);
		virtual ~WGlRenderContext();
		virtual void init() override;
		virtual void swapBuffer() override;

		virtual VertexBufferHandle createVertexBuffer(size_t size, const void* data) override;
		virtual IndexBufferHandle createIndexBuffer()override;
		virtual ShaderHandle createShader() override;
		virtual ProgramHandle createProgram() override;
		virtual TextureHandle createTexture() override;
		virtual FrameBufferHandle createFrameBuffer() override;

	private:
		std::vector<GlVertexBuffer> m_vertexBuffers; // These vectors need replacing when I create custom allocators
		std::vector<IndexBufferHandle> m_indexBuffers;
		std::vector<ShaderHandle> m_shaders;
		std::vector<ProgramHandle> m_programs;

		HWND m_owningWindow;
		HGLRC m_renderContext;
		HDC m_deviceContext;
	};
}