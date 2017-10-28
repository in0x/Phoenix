#pragma once

#include "RenderDefinitions.hpp"

#include "../Math/Vec3.hpp"
#include "../Math/Vec4.hpp"
#include "../Math/Matrix3.hpp"
#include "../Math/Matrix4.hpp"

namespace Phoenix
{
	class RIResource
	{
	// NOTE(Phil): I want to have this so that I can
	// introduce shared Resource behaviour later. 
	// Especially interesting if e.g. we need to have
	// guarenteed init on a specific thread.
	public:
		virtual ~RIResource() {}
	};

	class RIVertexBuffer : public RIResource
	{
	};

	class RIIndexBuffer : public RIResource
	{
	};

	class RIVertexShader : public RIResource
	{
	};

	class RIFragmentShader : public RIResource
	{
	};

	class RIProgram : public RIResource
	{
	};

	class RITexture2D : public RIResource
	{
	public:
		uint32_t m_width;
		uint32_t m_height;
	};

	class RIRenderTarget : public RIResource
	{
	};

	template<class T>
	class RIUniform : public RIResource
	{
		RIUniform(const T& value, const char* name)
			: m_value(value)
			, m_name(nullptr)
			, m_nameHash(HashFNV<const char*>()(name))
		{
		}

		T m_value;
		virtual const char* getName() { return nullptr; };
		size_t size() const { return sizeof(T); }

	private:
		Hash m_nameHash;
	};

	using RIUniformInt = RIUniform<int32_t>;
	using RIUniformFloat = RIUniform<float>;
	using RIUniformVec3 = RIUniform<Vec3>;
	using RIUniformVec4 = RIUniform<Vec4>;
	using RIUniformMat3 = RIUniform<Matrix3>;
	using RIUniformMat4 = RIUniform<Matrix4>;
}