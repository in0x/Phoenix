#pragma once

#include <Render/RIDevice.hpp>
#include <Render/RIResourceHandles.hpp>
#include <Render/RIResourceContainer.hpp>

namespace Phoenix
{
	class RIWGlDetails;
	class GlFramebuffer;
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
		
		virtual Texture2DHandle		 createTexture2D(const TextureDesc& desc) override;
		
		virtual TextureCubeHandle	 createTextureCube(const TextureDesc& desc) override;
		
		virtual RenderTargetHandle	 createRenderTarget(const RenderTargetDesc& desc) override;
		
		virtual UniformHandle		 createUniform(const char* name, EUniformType type, EUniformIsArray isArray) override;

		virtual ConstantBufferHandle createConstantBuffer(const ConstantBufferDesc& desc) override;
		
	private:
		RIOpenGLResourceStore* m_resources;

		bool RIDeviceOpenGL::attachColors(const RenderTargetDesc& desc, GlFramebuffer* fb);	
		bool attachifValid(Texture2DHandle tex, GlFramebuffer* fb, RenderTargetDesc::EAttachment attachment);
	};
}