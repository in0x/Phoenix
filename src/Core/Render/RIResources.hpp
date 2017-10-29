#pragma once

#include "../FNVHash.hpp"

#include "../Math/Vec3.hpp"
#include "../Math/Vec4.hpp"
#include "../Math/Matrix3.hpp"
#include "../Math/Matrix4.hpp"

namespace Phoenix
{
	// Could also have a virtual init and release in each type, would allow init without Device?

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
	
	template <class T>
	struct DefaultInitHelper
	{
		constexpr static T get();
	};

	template <>
	struct DefaultInitHelper<int32_t>
	{
		constexpr static int32_t get() { return 0; }
	};

	template <>
	struct DefaultInitHelper<float>
	{
		constexpr static float get() { return 0.f; }
	};

	template <>
	struct DefaultInitHelper<Vec3>
	{
		constexpr static Vec3 get() { return Vec3{}; }
	};

	template <>
	struct DefaultInitHelper<Vec4>
	{
		constexpr static Vec4 get() { return Vec4{}; }
	};

	template <>
	struct DefaultInitHelper<Matrix3>
	{
		constexpr static Matrix3 get() { return Matrix3{}; }
	};

	template <>
	struct DefaultInitHelper<Matrix4>
	{
		constexpr static Matrix4 get() { return Matrix4{}; }
	};

	template<class T>
	class RIUniform : public RIResource
	{
	public:
		RIUniform()
			: m_value(DefaultInitHelper<T>::get())
			, m_nameHash(0)
		{
		}
		
		RIUniform(const T& value, const char* name)
			: m_value(value)
			, m_nameHash(HashFNV<const char*>()(name))
		{
		}

		size_t size() const { return sizeof(T); }

		T m_value;
		FNVHash m_nameHash;
	};

	using RIUniformInt =   RIUniform<int32_t>;
	using RIUniformFloat = RIUniform<float> ;
	using RIUniformVec3 =  RIUniform<Vec3> ;
	using RIUniformVec4 =  RIUniform<Vec4> ;
	using RIUniformMat3 =  RIUniform<Matrix3> ;
	using RIUniformMat4 =  RIUniform<Matrix4> ;
}