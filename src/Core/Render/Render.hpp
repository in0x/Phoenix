#pragma once

#include <limits>
#include <stdint.h>
#include <array>

#undef max

namespace Phoenix
{
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
			AttributeProperty::Value m_property; // The kind of property the attrib sets (position, normal, color, etc.)
			AttributeType::Value m_type; // The datatype of the property (float, int, etc.)
			uint8_t m_numElements; // The number of elements (e.g. 3 for a Vec3)
		};

		struct Data
		{
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
		using List = ShaderHandle[Shader::Type::Count];
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

	struct DrawItem
	{
		// StateGroup + Command = DrawItem
	};
}