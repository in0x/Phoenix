#pragma once

#include <stdint.h>
#include <array>

#include "RIResourceHandles.h"

namespace Phoenix
{
	enum class ERenderApi
	{
		Gl,
		NoOp
	};

	// Should be subclassed to pass to specific RenderContext implementations
	class RenderInit
	{
	public:
		ERenderApi getApiType()
		{
			return m_apiType;
		}

	protected:
		RenderInit(ERenderApi apiType = ERenderApi::NoOp)
			: m_apiType(apiType)
		{}

		virtual ~RenderInit() {}

		ERenderApi m_apiType;
	};

	class RIContext;

	typedef void(*SubmitFptr)(RIContext*, const void*);

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
		Mat4
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
				, m_bNormalize(m_bNormalize)
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
			if (auto currentAttrib = has(decl.m_property))
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
			for (auto& attrib : m_attribs)
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
		Quads
	};

	enum class EDepth
	{
		Enable,
		Disable
	};

	enum class EPixelFormat
	{
		None,
		R8G8B8A8,
		R8G8B8,
		Depth32F,
		Depth16I,
		Stencil8I,
		Depth24IStencil8I
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
		Texture2DHandle attachments[4]; // DepthStencil is always preferred if set.

		RenderTargetDesc()
		{
			for (size_t i = 0; i < 4; ++i)
			{
				attachments[i].invalidate();
			}
		}

		enum EAttachment
		{
			Color = 0,
			Depth,
			Stencil,
			DepthStencil
		};
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