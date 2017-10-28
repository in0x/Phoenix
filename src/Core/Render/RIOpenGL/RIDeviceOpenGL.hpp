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
		virtual IndexBufferHandle	 createIndexBuffer(size_t size, uint32_t count, const void* data) override;
		virtual VertexShaderHandle	 createVertexShader(const char* source) override;
		virtual FragmentShaderHandle createFragmentShader(const char* source) override;
		virtual ProgramHandle		 createProgram(VertexShaderHandle vsHandle, FragmentShaderHandle fsHandle) override;
		virtual IntUniformHandle	 createIntUniform(const char* name, int32_t value) override;
		virtual FloatUniformHandle   createFloatUniform(const char* name, float value) override;
		virtual Vec3UniformHandle	 createVec3Uniform(const char* name, const Vec3& value) override;
		virtual Vec4UniformHandle	 createVec4Uniform(const char* name, const Vec4& value) override;
		virtual Mat3UniformHandle	 createMat3Uniform(const char* name, const Matrix3& value) override;
		virtual Mat4UniformHandle	 createMat4Uniform(const char* name, const Matrix4& value) override;
		virtual Texture2DHandle		 createTexture2D(const TextureDesc& desc) override;
		virtual RenderTargetHandle	 createRenderTarget(const RenderTargetDesc& desc) override;

	private:
		RIOpenGLResourceStore* m_resources;
	};
}