#pragma once

#include <limits>

namespace Phoenix
{
	enum class EResourceHandleType
	{
		EVertexBuffer,
		EIndexBuffer,
		EVertexShader,
		EFragmentShader,
		EProgram,
		EUniform,
		EUniformBuffer,
		ETexture2D,
		ETextureCube,
		ERenderTarget
	};

	template<class IndexType, IndexType MaxValue, EResourceHandleType handleType>
	class ResourceHandle
	{
	public:
		static_assert(MaxValue < std::numeric_limits<IndexType>::max(), "ResourceHandle MaxValue must be alteast one smaller than the max value of IndexType to allow for an invalid value");

		explicit ResourceHandle()	
			: m_idx(invalidValue())
		{}

		IndexType m_idx;

		static constexpr IndexType maxValue()
		{
			return MaxValue;
		}

		static constexpr IndexType invalidValue()
		{
			return MaxValue + 1;
		}

		void invalidate()
		{
			m_idx = invalidValue();
		}

		bool isValid() const
		{
			return m_idx != invalidValue();
		}
	};

	using VertexBufferHandle =	 ResourceHandle<size_t, 1024, EResourceHandleType::EVertexBuffer>;
	using IndexBufferHandle =	 ResourceHandle<size_t, 1024, EResourceHandleType::EIndexBuffer>;
	using VertexShaderHandle =	 ResourceHandle<size_t, 1024, EResourceHandleType::EVertexShader>;
	using FragmentShaderHandle = ResourceHandle<size_t, 1024, EResourceHandleType::EFragmentShader>;
	using ProgramHandle =		 ResourceHandle<size_t, 1024, EResourceHandleType::EProgram>;
	using UniformHandle =		 ResourceHandle<size_t, 1024, EResourceHandleType::EUniform>;
	using UniformBufferHandle =  ResourceHandle<size_t, 1024, EResourceHandleType::EUniformBuffer>;
	using Texture2DHandle =		 ResourceHandle<size_t, 1024, EResourceHandleType::ETexture2D>;
	using TextureCubeHandle =	 ResourceHandle<size_t, 1024, EResourceHandleType::ETextureCube>;
	using RenderTargetHandle =	 ResourceHandle<size_t, 1024, EResourceHandleType::ERenderTarget>;
}