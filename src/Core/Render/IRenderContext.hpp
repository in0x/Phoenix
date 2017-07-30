#pragma once
#include <limits>
#include <stdint.h>
#undef max // TODO(Phil): Figure out how to stop this
#include "../Logger.hpp"

namespace Phoenix
{

#define HANDLE(name, size) \
		struct name \
		{ \
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

	namespace Shader
	{
		enum Type
		{
			Vertex,
			Geometry,
			Fragment,
			Compute,
			None,
		};

		using List = ShaderHandle[4];
	}

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

	namespace VertexFormat
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

	class VertexBufferFormat
	{
	public:
		VertexBufferFormat()
			: m_count(0)
		{}
		
		void add(const VertexFormat::Decl& format, const VertexFormat::Data& data)
		{
			if (m_count >= AttributeType::Count)
			{
				Logger::warning("Maximum amount of attributes exceeded.");
				return;
			}
			
			m_inputDecl[m_count] = format;
			m_inputData[m_count] = data;
			m_count++;
		}

		VertexFormat::Data m_inputData[AttributeType::Count];
		VertexFormat::Decl m_inputDecl[AttributeType::Count];
		size_t m_count;
	};

	class IRenderContext
	{
	public: 
		virtual void init() = 0;
		virtual VertexBufferHandle createVertexBuffer(VertexBufferFormat format) = 0;
		virtual IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data) = 0;
		virtual ShaderHandle createShader(const char* source, Shader::Type shaderType) = 0;
		virtual ProgramHandle createProgram(const Shader::List& shaders) = 0;
		virtual TextureHandle createTexture() = 0;
		virtual FrameBufferHandle createFrameBuffer() = 0;
		virtual void swapBuffer() = 0;
	};
}