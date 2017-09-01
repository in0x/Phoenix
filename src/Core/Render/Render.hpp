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
			size idx = invalidValue; \
			bool isValid() const { return idx != invalidValue; } \
		} \

	HANDLE(VertexBufferHandle, uint16_t);
	HANDLE(IndexBufferHandle, uint16_t);
	HANDLE(ShaderHandle, uint16_t);
	HANDLE(ProgramHandle, uint16_t);
	HANDLE(TextureHandle, uint16_t);
	HANDLE(FrameBufferHandle, uint16_t);
	HANDLE(UniformHandle, uint16_t);
	HANDLE(TextureListHandle, uint16_t);

	class IRenderContext;

	typedef void(*SubmitFptr)(IRenderContext*, const void*);

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

	namespace AttributeProperty
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

	namespace AttributeType
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
	inline const char* attribTypeToName(AttributeProperty::Value type)
	{
		static const char* const names[AttributeProperty::Count] =
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
				: m_property(AttributeProperty::Count)
				, m_type(AttributeType::Count)
				, m_numElements(0)
			{}


			Decl(AttributeProperty::Value type, AttributeType::Value size, uint8_t numElement)
				: m_property(type)
				, m_type(size)
				, m_numElements(numElement)
			{}

			AttributeProperty::Value m_property; // The kind of property the attrib sets (position, normal, color, etc.)
			AttributeType::Value m_type; // The datatype of the property (float, int, etc.)
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
		std::array<VertexAttrib, AttributeProperty::Count> m_attribs;
		size_t m_index;

		VertexAttrib* has(AttributeProperty::Value attribType)
		{
			for (auto& attrib : m_attribs)
			{
				if (attrib.m_decl.m_property == attribType)
					return &attrib;
			}
			return nullptr;
		}
	};

	namespace Shader
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
		using List = std::array<ShaderHandle, Shader::Type::Count>;
	}

	namespace Uniform
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

	struct UniformInfo
	{
		char name[RenderConstants::c_maxUniformNameLenght];
		Uniform::Type type;
	};

	namespace Primitive
	{
		enum Type
		{
			Points,
			Lines,
			Triangles
		};
	};

	namespace Blend
	{
		enum Type
		{
			Opaque
		};
	}

	namespace Raster
	{
		enum Type
		{
			Placeholder
		};
	}

	namespace Depth
	{
		enum Type
		{
			Placeholder
		};
	}

	namespace Stencil
	{
		enum Type
		{
			Placeholder
		};
	}

	struct StateGroup
	{
		Blend::Type blend;
		Raster::Type raster;
		Depth::Type depth;
		Stencil::Type stencil;
		// Textures -> TextureListHandle
		// Unfiforms can also use a Resourcelist like mechanism
		ProgramHandle program;
	};
	
	//struct DrawItem
	//{
	//	// StateGroup + Command = DrawItem
	//	void* pCommand;
	//	SubmitFptr draw; // Drawfuncs should be registered to a table somewhere and then fetched via lookup using data from the command (i.e. a key (bitmask))
	//	StateGroup state;
	//};

	//template<class T>
	//DrawItem* compile(const StateGroup* stateGroupStack, uint32_t stateGroupCount, T command)
	//{
	//
	//}
}