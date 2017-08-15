#pragma once
#include <limits>
#include <stdint.h>
#include <array>
#undef max // TODO(Phil): Figure out how to stop this
#include "../Logger.hpp"

namespace Phoenix
{

#define HANDLE(name, size) \
		class name \
		{ \
		public: \
			static const size invalidValue = std::numeric_limits<size>::max(); \
			size idx = invalidValue; \
			bool isValid() const { return idx != invalidValue; } \
		}; \

	HANDLE(VertexBufferHandle, uint16_t)
	HANDLE(IndexBufferHandle, uint16_t)
	HANDLE(ShaderHandle, uint16_t)
	HANDLE(ProgramHandle, uint16_t)
	HANDLE(TextureHandle, uint16_t)
	HANDLE(FrameBufferHandle, uint16_t)
	HANDLE(UniformHandle, uint16_t)

	namespace AttributeType
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

	namespace AttributeSize
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
	inline const char* attribTypeToName(AttributeType::Value type)
	{
		static const char* const names[AttributeType::Count] =
		{
			"Position",
			"Normal",
			"Color",
			"Bitangent",
			"TexCoord",
		};

		return names[type];
	}

	struct VertexAttrib
	{
		struct Decl
		{
			Decl()
				: m_type(AttributeType::Count)
				, m_size(AttributeSize::Count)
				, m_numElements(0)
			{}


			Decl(AttributeType::Value type, AttributeSize::Value size, uint8_t numElement)
				: m_type(type)
				, m_size(size)
				, m_numElements(numElement)
			{}

			AttributeType::Value m_type;
			AttributeSize::Value m_size;
			uint8_t m_numElements;
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

			size_t m_size;
			size_t m_count;
			const void* m_data;
			bool m_bNormalize;
		};

		Decl m_decl;
		Data m_data;
	};

	class VertexBufferFormat
	{
	public:
		VertexBufferFormat() : m_attribs(), m_index(0) {}
		
		void add(const VertexAttrib::Decl& decl, const VertexAttrib::Data& data)
		{
			if (auto currentAttrib = has(decl.m_type))
			{
				*currentAttrib = {decl, data};
				return;
			}

			m_attribs[m_index] = {decl, data};
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
		std::array<VertexAttrib, AttributeType::Count> m_attribs;
		size_t m_index;

		VertexAttrib* has(AttributeType::Value attribType)
		{
			for (auto& attrib : m_attribs)
			{
				if (attrib.m_decl.m_type == attribType) 
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

		using List = ShaderHandle[Shader::Type::Count];
	}

	class IRenderContext
	{
	public: 
		virtual void init() = 0;
		virtual VertexBufferHandle createVertexBuffer(const VertexBufferFormat& format) = 0;
		virtual IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data) = 0;
		virtual ShaderHandle createShader(const char* source, Shader::Type shaderType) = 0;
		virtual ProgramHandle createProgram(const Shader::List& shaders) = 0;
		virtual TextureHandle createTexture() = 0;
		virtual FrameBufferHandle createFrameBuffer() = 0;
		virtual UniformHandle createUniform() = 0;

		//TODO(Phil): Destruction functions / release of resources on self destruction

		virtual void swapBuffer() = 0;
	};
}