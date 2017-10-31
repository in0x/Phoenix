#include "RIBackend.hpp"
#include "RIOpenGL/RenderInitWGL.hpp"

namespace Phoenix
{
	RIBackend::RIBackend(const RenderInit* initValues)
	{
	
	}

	template <class UniformType, class ValueType>
	void initUniform(UniformType* uniform, const char* name, const ValueType& value)
	{
		uniform->m_value = value;
		uniform->m_nameHash = HashFNV<const char*>()(name);
	}

	IntUniformHandle RIBackend::createIntUniform(const char* name, int32_t value)
	{
		auto handle = m_intUniforms.allocateResource();
		auto* uniform = m_intUniforms.getResource(handle);

		initUniform(uniform, name, value);
		return handle;
	}

	FloatUniformHandle RIBackend::createFloatUniform(const char* name, float value)
	{
		auto handle = m_floatUniforms.allocateResource();
		auto* uniform = m_floatUniforms.getResource(handle);

		initUniform(uniform, name, value);
		return handle;
	}
	
	Vec3UniformHandle RIBackend::createVec3Uniform(const char* name, const Vec3& value)
	{
		auto handle = m_vec3Uniforms.allocateResource();
		auto* uniform = m_vec3Uniforms.getResource(handle);

		initUniform(uniform, name, value);
		return handle;
	}
	
	Vec4UniformHandle RIBackend::createVec4Uniform(const char* name, const Vec4& value)
	{
		auto handle = m_vec4Uniforms.allocateResource();
		auto* uniform = m_vec4Uniforms.getResource(handle);

		initUniform(uniform, name, value);
		return handle;
	}
	
	Mat3UniformHandle RIBackend::createMat3Uniform(const char* name, const Matrix3& value)
	{
		auto handle = m_mat3Uniforms.allocateResource();
		auto* uniform = m_mat3Uniforms.getResource(handle);

		initUniform(uniform, name, value);
		return handle;
	}

	Mat4UniformHandle RIBackend::createMat4Uniform(const char* name, const Matrix4& value)
	{
		auto handle = m_mat4Uniforms.allocateResource();
		auto* uniform = m_mat4Uniforms.getResource(handle);

		initUniform(uniform, name, value);
		return handle;
	}
}