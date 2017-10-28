#pragma once

#include <stdint.h>
#include <array>

//TODO(Phil): Rename to RIPublic.hpp

namespace Phoenix
{
	namespace ERenderApi
	{
		enum Type
		{
			Gl,
			DX11,
			NoOp
		};
	}

	// Should be subclassed to pass to specific RenderContext implementations
	class RenderInit
	{
	public:
		ERenderApi::Type getApiType()
		{
			return m_apiType;
		}

	protected:
		RenderInit(ERenderApi::Type apiType = ERenderApi::NoOp)
			: m_apiType(apiType)
		{}

		virtual ~RenderInit() {}

		ERenderApi::Type m_apiType;
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

	namespace EAttributeProperty
	{
		enum Value
		{
			Position,
			Normal,
			Color,
			Bitangent,
			TexCoord,
			Count,
		};
	}

	namespace EAttributeType
	{
		enum Value
		{
			Double,
			Float,
			Uint,
			Int,
			Count
		};
	}

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


			Decl(EAttributeProperty::Value type, EAttributeType::Value size, uint8_t numElement)
				: m_property(type)
				, m_type(size)
				, m_numElements(numElement)
			{}

			EAttributeProperty::Value m_property; // The kind of property the attrib sets (position, normal, color, etc.)
			EAttributeType::Value m_type; // The datatype of the property (float, int, etc.)
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
		std::array<VertexAttrib, EAttributeProperty::Count> m_attribs;
		size_t m_index;

		VertexAttrib* has(EAttributeProperty::Value attribType)
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

	typedef uint64_t Hash;

	inline Hash hashBytes(const void* data, size_t size, uint64_t offset = 14695981039346656037, uint64_t prime = 1099511628211)
	{
		const char* ptr = static_cast<const char*>(data);
		Hash hash = offset;
		
		for (size_t i = 0; i < size; ++i)
		{
			Hash next = static_cast<Hash>(ptr[i]);
			hash = (hash ^ next) * prime;
		}

		return hash;
	}

	// begin Hash.h
	
	template<uint64_t offset = 14695981039346656037, uint64_t prime = 1099511628211>
	struct HashFNVIterative
	{
		Hash m_hash;
	
	public:
		HashFNVIterative()
			: m_hash(offset)
		{}

		void add(const void* data, size_t size)
		{
			const char* ptr = static_cast<const char*>(data);

			for (size_t i = 0; i < size; ++i)
			{
				Hash next = static_cast<Hash>(ptr[i]);
				m_hash = (m_hash ^ next) * prime;
			}
		}

		Hash operator()()
		{
			return m_hash;
		}
	};

	template <class T>
	struct HashFNV
	{
		Hash operator()(const T& obj)
		{
			auto hash = std::hash<T>();
			return static_cast<uint64_t>(hash(obj));
		}
	};

	template <>
	struct HashFNV<const char*>
	{
		Hash operator()(const char* str)
		{
			return hashBytes(str, strlen(str));
		}
	};

	// end Hash.h

	namespace EPrimitive
	{
		enum Type
		{
			Points,
			Lines,
			Triangles,
			TriangleStrip,
			Quads
		};
	};

	namespace EBlend
	{
		enum Type
		{
			Opaque
		};
	}

	namespace ERaster
	{
		enum Type
		{
			Placeholder
		};
	}

	namespace EDepth
	{
		enum Type
		{
			Enable,
			Disable
		};
	}

	namespace EStencil
	{
		enum Type
		{
			Placeholder
		};
	}

	namespace EBuffer
	{
		enum Type
		{
			Color,
			Depth,
			Stencil 
		};
	}

	namespace ETexture
	{
		enum Format
		{
			Tex1D,
			Tex2D,
			Tex3D,
			CubeMap
		};
		
		enum Components
		{
			R,
			RG,
			RGB,
			RGBA,
			DEPTH
		};

		enum Filter
		{
			Nearest,
			Linear
		};
	}

	class TextureDesc
	{
	public:
		uint32_t width;
		uint32_t height;
		ETexture::Components components;
		ETexture::Filter minFilter;
		ETexture::Filter magFilter;
		uint8_t numMips;
	};

	struct CubemapData
	{
		enum Side
		{
			Right = 0, // xpos
			Left,	   // xneg
			Up,        // ypos
			Down,      // yneg
			Back,      // zpos
			Front      // zneg
		}; 

		const void* data[6];
	};

	struct RGBA
	{
		float r, g, b, a;
	};

	namespace ERenderAttachment
	{
		enum Type 
		{
			Color =   1 << 0,
			Stencil = 1 << 1,
			Depth =   1 << 2
		};
	}

	class RenderTargetDesc
	{
	public:
		RenderTargetDesc()
			: attachment(0)
			, width(0)
			, height(0)
		{}

		void AddAttachment(ERenderAttachment::Type type)
		{
			if (!(attachment & type))
			{
				attachment |= type;
			}
		}
		
		int32_t attachment;
		uint32_t width;
		uint32_t height;
	};

	struct UniformInfo;
	
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