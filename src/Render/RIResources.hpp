#pragma once

#include "RIDefs.hpp"
#include <Core/FNVHash.hpp>
#include <assert.h>

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
	public:
		RIIndexBuffer()
			: m_numElements(0)
		{}

		size_t m_numElements;
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

	class RITexture : public RIResource
	{
	public:
		RITexture()
			: m_pixelFormat(EPixelFormat::None)
			, m_numMips(0)
		{}

		EPixelFormat m_pixelFormat;
		uint8_t m_numMips;
	};

	class RITexture2D : public RITexture
	{
	public:
		RITexture2D()
			: m_width(0)
			, m_height(0)
		{}

		uint32_t m_width;
		uint32_t m_height;
	};

	class RITextureCube : public RITexture
	{
	public:
		RITextureCube()
			: m_size(0)
		{}

		uint32_t m_size;
	};

	class RIRenderTarget : public RIResource
	{
	};
	
	class RIUniform : public RIResource
	{
	public:
		enum { MAX_NAME_LENGTH = 32 };

		char m_debugName[MAX_NAME_LENGTH];
		EUniformType m_type;
		FNVHash m_nameHash;
	};
}