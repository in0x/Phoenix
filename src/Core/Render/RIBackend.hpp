#pragma once

#include "RIResources.hpp"
#include "RIResourceHandles.h"
#include "RIResourceContainer.hpp"

namespace Phoenix
{
	class Vec3;
	class Vec4;
	class Mat3;
	class Mat4;
	class RenderInit;

	// DONT IMPLEMENT ANY STATE SETTING OR COMMAND EXECUTING FUNCTIONALITY HERE, ALL THAT HAPPENS VIA COMMANDS

	class RIBackend // Really, this might be the only interface we need and then we put all the other functionality behind this
	{
	public:
		RIBackend(const RenderInit* initValues);
		virtual ~RIBackend() {}
		/*
			IntUniformHandle	 createIntUniform(const char* name, int32_t value);
			FloatUniformHandle   createFloatUniform(const char* name, float value);
			Vec3UniformHandle	 createVec3Uniform(const char* name, const Vec3& value);
			Vec4UniformHandle	 createVec4Uniform(const char* name, const Vec4& value);
			Mat3UniformHandle	 createMat3Uniform(const char* name, const Matrix3& value);
			Mat4UniformHandle	 createMat4Uniform(const char* name, const Matrix4& value);

			RIResourceContainer<RIUniformInt,	  IntUniformHandle,		128 > m_intUniforms;
			RIResourceContainer<RIUniformFloat,	  FloatUniformHandle,	128 > m_floatUniforms;
			RIResourceContainer<RIUniformVec3,	  Vec3UniformHandle,	128 > m_vec3Uniforms;
			RIResourceContainer<RIUniformVec4,	  Vec4UniformHandle,	128 > m_vec4Uniforms;
			RIResourceContainer<RIUniformMat3,	  Mat3UniformHandle,	128 > m_mat3Uniforms;
			RIResourceContainer<RIUniformMat4,	  Mat4UniformHandle,	128 > m_mat4Uniforms;*/
	};

	class RIBackendOpenGL : public RIBackend
	{
		RIBackendOpenGL() : RIBackend(nullptr)
		{
			//RIOpenGLResourceStore glResources;
			//RIWGLGlewSupport glew(&init);
			//IRIDevice* renderDevice = new RIDeviceOpenGL(&glResources);
		}
	};
}

