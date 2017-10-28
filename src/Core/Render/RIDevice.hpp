#pragma once

#include <stdint.h>
#include "RIResourceHandles.h"

namespace Phoenix
{
	class Vec3;
	class Vec4;
	class Matrix3;
	class Matrix4;
	class VertexBufferFormat;
	class TextureDesc;
	class RenderTargetDesc;

	class IRIDevice
	{
	public:
		virtual ~IRIDevice() {}

		virtual VertexBufferHandle	 createVertexBuffer(const VertexBufferFormat& format) = 0;
		virtual IndexBufferHandle	 createIndexBuffer(size_t size, uint32_t count, const void* data) = 0;
		virtual VertexShaderHandle	 createVertexShader(const char* source) = 0;
		virtual FragmentShaderHandle createFragmentShader(const char* source) = 0;
		virtual ProgramHandle		 createProgram(VertexShaderHandle vs, FragmentShaderHandle fs) = 0;
		virtual IntUniformHandle	 createIntUniform(const char* name, int32_t value) = 0;
		virtual FloatUniformHandle   createFloatUniform(const char* name, float value) = 0;
		virtual Vec3UniformHandle	 createVec3Uniform(const char* name, const Vec3& value) = 0;
		virtual Vec4UniformHandle	 createVec4Uniform(const char* name, const Vec4& value) = 0;
		virtual Mat3UniformHandle	 createMat3Uniform(const char* name, const Matrix3& value) = 0;
		virtual Mat4UniformHandle	 createMat4Uniform(const char* name, const Matrix4& value) = 0;
		virtual Texture2DHandle		 createTexture2D(const TextureDesc& desc) = 0;
		virtual RenderTargetHandle	 createRenderTarget(const RenderTargetDesc& desc) = 0;
	};
}
