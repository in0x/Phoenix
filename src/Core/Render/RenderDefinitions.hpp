#pragma once

#include <limits>
#include <stdint.h>
#include <array>

#undef max

// Goals:
// Command-based: The user submits commands that specify the desired action
// and required data which is then executed using the GPU. Commands can be 
// stored in buckets, which in turn can be sorted in order to group commands
// with similar data so as to reduce state changes.
//
// Stateless: To prevent the pitfalls of stateful APIs such as OpenGL, where
// a state set by previous call can affect the next call, the system should be
// stateless: Options set by the previous draw call should not have an effect on 
// the next. 

namespace Phoenix
{
	namespace RenderConstants
	{
		constexpr uint32_t c_maxUniformNameLenght = 128;
	}

#define HANDLE(name, size) \
		class name \
		{ \
		public: \
			static const size invalidValue = std::numeric_limits<size>::max(); \
			size idx; \
			bool isValid() const { return idx != invalidValue; } \
			static constexpr size maxValue() { return std::numeric_limits<size>::max() - 1; } \
		}; \
		inline name create##name(size value = name::invalidValue)\
		{ \
			name handle; \
			handle.idx = value; \
			return handle; \
		} \

#define HANDLE_CUSTOM_MAXVAL(name, size, maxVal) \
		static_assert(maxVal < std::numeric_limits<size>::max(), "Handle maxVal has to be smaller than largest value of type size"); \
		class name \
		{ \
		public: \
			static const size invalidValue = std::numeric_limits<size>::max(); \
			size idx; \
			bool isValid() const { return idx != invalidValue; } \
			static constexpr size maxValue() { return maxVal; } \
		}; \
		inline name create##name(size value = name::invalidValue)\
		{ \
			name handle; \
			handle.idx = value; \
			return handle; \
		} \

	HANDLE(VertexBufferHandle, uint16_t);
	HANDLE(IndexBufferHandle, uint16_t);
	HANDLE(ShaderHandle, uint16_t);
	HANDLE(ProgramHandle, uint16_t);
	HANDLE(TextureHandle, uint16_t);
	HANDLE(FrameBufferHandle, uint8_t);
	HANDLE(UniformHandle, uint8_t);
	
	class IRenderBackend;

	typedef void(*SubmitFptr)(IRenderBackend*, const void*);

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

	namespace ERenderApi
	{
		enum Type
		{
			Gl,
			None
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

		size_t m_resourceListMemoryBytes;

	protected:
		RenderInit(size_t resourceListMemoryBytes, ERenderApi::Type apiType = ERenderApi::None)
			: m_apiType(apiType)
			, m_resourceListMemoryBytes(resourceListMemoryBytes)
		{}

		virtual ~RenderInit() {}

		ERenderApi::Type m_apiType;
	};

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

	// NOTE(Phil): This should be moved.
	inline const char* attribTypeToName(EAttributeProperty::Value type)
	{
		static const char* const names[EAttributeProperty::Count] =
		{
			"Position",
			"Normal",
			"Color",
			"Bitangent",
			"TexCoord",
		};

		return names[type];
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

	namespace EShader
	{
		enum Type
		{
			Vertex,
			Geometry,
			Fragment,
			Compute,
			Count,
		};

		// A set of shaders to be compiled into a program. 
		// Some shaders may be omitteted if they are not required to create a valid shader program.
		//using List = ShaderHandle[Shader::Type::Count];
		using List = std::array<ShaderHandle, EShader::Type::Count>;
		inline List createList()
		{
			ShaderHandle invalidHandle = createShaderHandle();
			return{ invalidHandle, invalidHandle, invalidHandle, invalidHandle };
		}
	}

	typedef uint64_t Hash_t;

	inline Hash_t hashBytes(const void* data, size_t size, uint64_t offset = 14695981039346656037, uint64_t prime = 1099511628211)
	{
		const char* ptr = static_cast<const char*>(data);
		Hash_t hash = offset;
		
		for (size_t i = 0; i < size; ++i)
		{
			Hash_t next = static_cast<Hash_t>(ptr[i]);
			hash = (hash ^ next) * prime;
		}

		return hash;
	}

	template<uint64_t offset = 14695981039346656037, uint64_t prime = 1099511628211>
	struct HashFNVIterative
	{
		Hash_t m_hash;
	
	public:
		HashFNVIterative()
			: m_hash(offset)
		{}

		void add(const void* data, size_t size)
		{
			const char* ptr = static_cast<const char*>(data);

			for (size_t i = 0; i < size; ++i)
			{
				Hash_t next = static_cast<Hash_t>(ptr[i]);
				m_hash = (m_hash ^ next) * prime;
			}
		}

		Hash_t operator()()
		{
			return m_hash;
		}
	};

	template <class T>
	struct HashFNV
	{
		Hash_t operator()(const T& obj)
		{
			auto hash = std::hash<T>();
			return static_cast<uint64_t>(hash(obj));
		}
	};

	template <>
	struct HashFNV<const char*>
	{
		Hash_t operator()(const char* str)
		{
			return hashBytes(str, strlen(str));
		}
	};

	namespace EUniform
	{
		enum Type
		{
			Float,
			Int,
			Vec3,
			Vec4,
			Mat3,
			Mat4,
			Count
		};
	}

	namespace EPrimitive
	{
		enum Type
		{
			Points,
			Lines,
			Triangles
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

	struct RGBA
	{
		float r, g, b, a;
	};

	template <class Resource> 
	struct ResourceList
	{
		Resource* m_resources;
		size_t m_count;
	};

	struct UniformInfo;
	typedef ResourceList<const UniformInfo*> UniformList;

	struct StateGroup
	{
		EBlend::Type blend;
		ERaster::Type raster;
		EDepth::Type depth;
		EStencil::Type stencil;
		// Textures -> TextureListHandle
		UniformList uniforms;
		ProgramHandle program;
	};

	typedef StateGroup* StateGroupStack;
	// Somehow make sure that when the user gets a stategroup stack one default is there per default

	// Compiles all StateGroups into one complete StateGroup. Interpretation
	// walks from left to right -> A setting at the front overrides a setting at the back.
	inline StateGroup compile(StateGroupStack states, uint32_t stateCount)
	{
		StateGroup state;

		for (size_t i = 0; i < stateCount; ++i)
		{
			const StateGroup& current = states[i];
			// Set each setting if not set already;
		}

		return state;
	}
}