#pragma once
#include <limits>
#include <stdint.h>
#include <array>
#include <memory>
#include <vector>
#include <string>

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
			const void* m_data; // A non-owning reference to the data fill the buffer with
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

		// NOTE(Phil): This is probably going to live in the frontend, i.e. not in the Context, but in 
		// some kind of Renderer class, which will be recieving commands, sorting them and submitting them
		// to the context. This way the shared features of Uniforms (name and type) can be handled seperatly
		// from the api-specific implementations (OpenGL uniforms, DirectX global variables etc.). 
		class Registry
		{
		public:
			Registry(uint16_t maxUniforms)
				: m_uniforms(maxUniforms)
			{
			
			}
			
			UniformHandle add(const char* name, Uniform::Type type, const void* data)
			{
				UniformHandle handle;

				if (strlen(name) > s_maxNameLength)
				{
					Logger::error("Uniform name exceeds max length");
					return handle;
				}

				m_uniforms.emplace_back(name, type);
				handle.idx = m_uniforms.size() - 1;
				return handle;
			}
		
			void update(UniformHandle handle, const void* data)
			{

			}
		
		private:
			static constexpr uint8_t s_maxNameLength = 32;

			struct Info
			{
				Info() : m_name(), m_type(Uniform::Type::Count) {}
				Info(const char* name, Uniform::Type type) : m_name(), m_type(type) { strncpy(m_name, name, strlen(name)); }
				char m_name[s_maxNameLength];
				Uniform::Type m_type;
			};

			std::vector<Info> m_uniforms;
		};
	}

	// A RenderContext represents an object that can be used to perform actions 
	// on rendering API (e.g. OpenGL or DirectX).  
	class IRenderContext
	{
	public: 
		virtual ~IRenderContext() {}
		virtual void init() = 0;
		virtual VertexBufferHandle createVertexBuffer(const VertexBufferFormat& format) = 0;
		virtual IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data) = 0;
		virtual ShaderHandle createShader(const char* source, Shader::Type shaderType) = 0;
		virtual ProgramHandle createProgram(const Shader::List& shaders) = 0;
		virtual TextureHandle createTexture() = 0;
		virtual FrameBufferHandle createFrameBuffer() = 0;
		virtual UniformHandle createUniform(const char* name, Uniform::Type type) = 0;

		//TODO(Phil): Destruction functions / release of resources on self destruction

		virtual void swapBuffer() = 0;
	};

	// NOTE(Phil): This is what the using code should be talking to eventually.
	// Right now, this functions are just going to forward their argument, but 
	// the frontend can und should eventually be extended to include key- 
	// and bucket-based sorting. Maybe, instead of using an interface for 
	// the context, the backend funtions can be implemented seperately and 
	// instead just use callbacks to hook up to the frontend.
	namespace Renderer
	{
		static std::unique_ptr<IRenderContext> sg_context;

		/*namespace API
		{
			enum Type
			{
				OpenGL
			};
		};*/

		/*void init(API::Type api)
		{
			switch (api)
			{
				case API::OpenGL
				{
					sg_context = std::make_unique<>
				}
			}
		}*/
		
		template <class Handle>
		void checkBufferValid(Handle handle)
		{
			if (!handle.isValid())
			{
				Logger::error("Failed to create VertexBuffer");
			}
		}

		inline VertexBufferHandle createVertexBuffer(const VertexBufferFormat& format)
		{
			auto bufferHandle = sg_context->createVertexBuffer(format);
			checkBufferValid(bufferHandle);
			return bufferHandle;
		}

		inline IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data)
		{
			auto bufferHandle = sg_context->createIndexBuffer(size, count, data);
			checkBufferValid(bufferHandle);
			return bufferHandle;
		}
		
		inline ShaderHandle createShader(const char* source, Shader::Type shaderType)
		{
			auto shaderHandle = sg_context->createShader(source, shaderType);
			checkBufferValid(shaderHandle);
			return shaderHandle;
		}
		
		inline ProgramHandle createProgram(const Shader::List& shaders)
		{
			auto programHandle = sg_context->createProgram(shaders);
			checkBufferValid(programHandle);
			return programHandle;
		}
		
		inline void submit()
		{
			sg_context->swapBuffer();
		}
	}
}