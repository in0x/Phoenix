#pragma once

#include <stdint.h>
#include <array>

#include "RIResourceHandles.hpp"

namespace Phoenix
{
	enum class ERenderApi
	{
		Gl
	};

	class IRIContext;

	typedef void(*SubmitFptr)(IRIContext*, const void*);

	template <class T>
	struct is_submittable
	{
		template <typename U> static std::true_type check(decltype(U::SubmitFunc)*);
		template <typename> static std::false_type check(...);

		typedef decltype(check<T>(0)) result;

	public:
		static const bool value = result::value;
	};

#define SUBMITTABLE() \
		const static SubmitFptr SubmitFunc \

	// Begin VertexBuffer.h

	enum class EAttributeProperty
	{
		Position,
		Normal,
		Color,
		Bitangent,
		TexCoord,
		Count,
	};

	enum class EAttributeType
	{
		Double,
		Float,
		Uint,
		Int,
		Count
	};

	enum class EUniformType
	{
		Int,
		Float,
		Vec3,
		Vec4,
		Mat3,
		Mat4,
		Sampler1D,
		Sampler2D,
		SamplerCube
	};

	// NOTE(Phil): Decl and data should likely be seperated into
	// two types, as multiple vertexbuffers may share the same
	// layout (this seems very likely), however I haven't thought
	// this through too far.
	struct VertexAttrib
	{
		struct Decl
		{
			Decl()
				: m_property(EAttributeProperty::Count)
				, m_type(EAttributeType::Count)
				, m_numElements(0)
			{}


			Decl(EAttributeProperty type, EAttributeType size, uint8_t numElement)
				: m_property(type)
				, m_type(size)
				, m_numElements(numElement)
			{}

			EAttributeProperty m_property; // The kind of property the attrib sets (position, normal, color, etc.)
			EAttributeType m_type; // The datatype of the property (float, int, etc.)
			uint8_t m_numElements; // The number of elements (e.g. 3 for a Vec3)
		};

		struct Data
		{
			Data()
				: m_size(0)
				, m_count(0)
				, m_data(nullptr)
				, m_bNormalize(false)
			{}

			Data(size_t size, size_t count, const void* data, bool bNormalize = false)
				: m_size(size)
				, m_count(count)
				, m_data(data)
				, m_bNormalize(bNormalize)
			{}

			size_t m_size; // The amount of memory taken up by a single element in the buffer (result of sizeof())
			size_t m_count; // The number of elements in the buffer 
			const void* m_data; // A non-owning reference to the data to fill the buffer with
			bool m_bNormalize; // Wether to normalize the data
		};

		Decl m_decl;
		Data m_data;
	};

	// Describes the layout and content of a buffer used as input for a vertex shader.
	class VertexBufferFormat
	{
	public:
		VertexBufferFormat() : m_attribs(), m_index(0) {}

		void add(const VertexAttrib::Decl& decl, const VertexAttrib::Data& data)
		{
			if (VertexAttrib* currentAttrib = has(decl.m_property))
			{
				*currentAttrib = { decl, data };
				return;
			}

			m_attribs[m_index] = { decl, data };
			m_index++;
		}

		size_t size() const
		{
			return m_index;
		}

		const VertexAttrib* at(size_t index) const
		{
			return &m_attribs[index];
		}

	private:
		std::array<VertexAttrib, static_cast<size_t>(EAttributeProperty::Count)> m_attribs;
		size_t m_index;

		VertexAttrib* has(EAttributeProperty attribType)
		{
			for (VertexAttrib& attrib : m_attribs)
			{
				if (attrib.m_decl.m_property == attribType)
					return &attrib;
			}
			return nullptr;
		}
	};

	// end VertexBuffer.h

	enum class EPrimitive
	{
		Points,
		Lines,
		Triangles,
		TriangleStrips
	};

	enum class EDepth
	{
		Enable,
		Disable
	};

	enum class EBlend
	{
		Enable,
		Disable
	};

	enum class EBlendFactor
	{
		Zero,
		One,
		SrcColor,
		OneMinusSrcColor,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstColor,
		OneMinusDstColor,
		DstAlpha,
		OneMinusDstAlpha
	};

	enum class EBlendOp
	{
		Add,
		Subtract_SD,
		Subtract_DS,
		Min,
		Max
	};

	class BlendState
	{
	public:
		// For don't care / disabled state.
		BlendState()
			: m_enabeld(EBlend::Disable)
			, m_blendOpRGB(EBlendOp::Add)
			, m_blendOpA(EBlendOp::Add)
			, m_factorSrcRGB(EBlendFactor::One)
			, m_factorSrcA(EBlendFactor::One)
			, m_factorDstRGB(EBlendFactor::Zero)
			, m_factorDstA(EBlendFactor::Zero)
		{}

		BlendState(EBlend enabled)
			: m_enabeld(enabled)
			, m_blendOpRGB(EBlendOp::Add)
			, m_blendOpA(EBlendOp::Add)
			, m_factorSrcRGB(EBlendFactor::One)
			, m_factorSrcA(EBlendFactor::One)
			, m_factorDstRGB(EBlendFactor::Zero)
			, m_factorDstA(EBlendFactor::Zero)
		{}

		// Convinience constructor if we want enabled but not separate equations.
		BlendState(EBlendOp opBoth, EBlendFactor factorSrcBoth, EBlendFactor factorABoth)
			: m_enabeld(EBlend::Enable)
			, m_blendOpRGB(opBoth)
			, m_blendOpA(opBoth)
			, m_factorSrcRGB(factorSrcBoth)
			, m_factorSrcA(factorABoth)
			, m_factorDstRGB(factorSrcBoth)
			, m_factorDstA(factorABoth)
		{}

		EBlend m_enabeld;

		EBlendOp m_blendOpRGB;
		EBlendOp m_blendOpA;

		EBlendFactor m_factorSrcRGB;
		EBlendFactor m_factorSrcA;
		
		EBlendFactor m_factorDstRGB;
		EBlendFactor m_factorDstA;
	};

	enum class EPixelFormat
	{
		None,
		R8,
		RG8,
		R8G8B8,
		R8G8B8A8,
		RGB16F,
		RGBA16F,
		RGB32F,
		RGBA32F,
		Depth32F,
		Depth16I,
		Stencil8I,
		//Depth24IStencil8I,
	};

	enum class ETextureFilter
	{
		Nearest,
		Linear
	};

	enum class ETextureWrap
	{
		ClampToEdge,
		Repeat,
		MirroredRepeat
	};

	class TextureDesc
	{
	public:
		class TextureDesc()
			: width(0)
			, height(0)
			, pixelFormat(EPixelFormat::None)
			, minFilter(ETextureFilter::Linear)
			, magFilter(ETextureFilter::Linear)
			, wrapU(ETextureWrap::ClampToEdge)
			, wrapV(ETextureWrap::ClampToEdge)
			, wrapW(ETextureWrap::ClampToEdge)
			, numMips(0)
		{}

		uint32_t width;
		uint32_t height;
		EPixelFormat pixelFormat;
		ETextureFilter minFilter;
		ETextureFilter magFilter;
		ETextureWrap wrapU;
		ETextureWrap wrapV;
		ETextureWrap wrapW;
		uint8_t numMips;
	};

	enum ETextureCubeSide
	{
		XPositive,
		XNegative,
		YPositive,
		YNegative,
		ZPositive,
		ZNegative
	};

	struct RGBA
	{
		float r, g, b, a;
	};

	class RenderTargetDesc
	{
	public:
		enum EAttachment
		{
			Color0 = 0,
			Color1,
			Color2,
			Color3,
			Color4,
			Color5,
			NumMaxColors,
			Depth,
			Stencil,
			NumAttachments, // NOTE(Phil): Always have this as second too last to counteract that we have NumMaxColors as well.
			DepthStencil,
		};

		// Attachments.
		Texture2DHandle colorAttachs[NumMaxColors];
		Texture2DHandle depthAttach; 
		Texture2DHandle stencilAttach; 
		Texture2DHandle depthStencilAttach; // DepthStencil should always be preferred if set.

		RenderTargetDesc()
		{
			for (size_t i = 0; i < 4; ++i)
			{
				colorAttachs[i].invalidate();
			}
			depthAttach.invalidate();
			stencilAttach.invalidate();
			depthStencilAttach.invalidate();
		}
	};

	struct UniformInfo;

	class PipelineState
	{

	};

	//struct StateGroup
	//{
	//	EBlend::Type blend;
	//	ERaster::Type raster;
	//	EDepth::Type depth;
	//	EStencil::Type stencil;
	//	TextureHandle* textures;
	//	size_t textureCount;
	//	UniformHandle* uniforms;
	//	size_t uniformCount;
	//	ProgramHandle program;
	//};

	//struct CStateGroup
	//{
	//	EBlend::Type blend;
	//	ERaster::Type raster;
	//	EDepth::Type depth;
	//	EStencil::Type stencil;
	//	ProgramHandle program;
	//	UniformInfo* uniforms;
	//	size_t uniformCount;
	//	UniformInfo* textureLocations;
	//	TextureHandle* textures;
	//	size_t textureCount;
	//};

	//typedef StateGroup* StateGroupStack;
	//// Somehow make sure that when the user gets a stategroup stack one default is there per default

	//// Compiles all StateGroups into one complete StateGroup. Interpretation
	//// walks from left to right -> A setting at the front overrides a setting at the back.
	//inline StateGroup compile(StateGroupStack states, uint32_t stateCount)
	//{
	//	StateGroup state;

	//	for (size_t i = 0; i < stateCount; ++i)
	//	{
	//		const StateGroup& current = states[i];
	//		// Set each setting if not set already;
	//	}

	//	return state;
	//}
}