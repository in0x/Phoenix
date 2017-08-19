#include "Renderer.hpp"
#include "IRenderContext.hpp"

namespace Phoenix
{
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

	VertexBufferHandle Renderer::createVertexBuffer(const VertexBufferFormat& format)
	{
		auto bufferHandle = m_pContext->createVertexBuffer(format);
		checkBufferValid(bufferHandle);
		return bufferHandle;
	}

	IndexBufferHandle Renderer::createIndexBuffer(size_t size, uint32_t count, const void* data)
	{
		auto bufferHandle = m_pContext->createIndexBuffer(size, count, data);
		checkBufferValid(bufferHandle);
		return bufferHandle;
	}

	ShaderHandle Renderer::createShader(const char* source, Shader::Type shaderType)
	{
		auto shaderHandle = m_pContext->createShader(source, shaderType);
		checkBufferValid(shaderHandle);
		return shaderHandle;
	}

	ProgramHandle Renderer::createProgram(const Shader::List& shaders)
	{
		auto programHandle = m_pContext->createProgram(shaders);
		checkBufferValid(programHandle);
		return programHandle;
	}

	void Renderer::draw(const Commands::DrawIndexed& command)
	{
		/*GLenum primtiveType = GL_TRIANGLES;

		switch (drawCmd.primtiveType)
		{
		case Primitive::Triangles:
		{
		primtiveType = GL_TRIANGLES;
		break;
		}
		case Primitive::Lines:
		{
		primtiveType = GL_LINES;
		break;
		}
		case Primitive::Points:
		{
		primtiveType = GL_POINTS;
		break;
		}
		default:
		{
		Logger::warning("PrimitiveType unsupported, reverting to default triangles");
		assert(false);
		}
		}

		GlShaderProgram* glProgram = static_cast<GlShaderProgram*>(command->drawItem->states->shaderProgram);
		glUseProgram(glProgram->m_nativeHandle);

		glDrawElements(primtiveType, drawCmd.count, GL_UNSIGNED_INT, (GLvoid*)(sizeof(unsigned int) * drawCmd.startIndex));*/
	}

	void Renderer::submit()
	{
		m_pContext->swapBuffer();
	}

}