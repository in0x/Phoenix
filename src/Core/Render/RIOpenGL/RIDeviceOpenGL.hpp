#pragma once

#include "../RIDevice.hpp"
#include "../RIResourceHandles.h"
#include "../RIResourceContainer.hpp"

namespace Phoenix
{
	class RIWGlDetails;
	class GlVertexBuffer;
	class RenderInit;
	struct RIOpenGLResourceStore;
	
	class RIDeviceOpenGL : public IRIDevice
	{
	public:
		RIDeviceOpenGL(RIOpenGLResourceStore* resources);
		virtual ~RIDeviceOpenGL();

		virtual VertexBufferHandle	 createVertexBuffer(const VertexBufferFormat& format) override;
		virtual IndexBufferHandle	 createIndexBuffer(size_t elementSizeBytes, size_t count, const void* data) override;
		virtual VertexShaderHandle	 createVertexShader(const char* source) override;
		virtual FragmentShaderHandle createFragmentShader(const char* source) override;
		virtual ProgramHandle		 createProgram(VertexShaderHandle vsHandle, FragmentShaderHandle fsHandle) override;
		virtual Texture2DHandle		 createTexture2D(const TextureDesc& desc, const char* name) override;
		virtual TextureCubeHandle	 createTextureCube(const TextureDesc& desc) override;
		virtual RenderTargetHandle	 createRenderTarget(const RenderTargetDesc& desc) override;
		virtual UniformHandle		 createUniform(const char* name, EUniformType type) override;

	private:
		RIOpenGLResourceStore* m_resources;
	};
}