#pragma once

#include "Render.hpp"

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
	class IRenderContext;

	// NOTE(Phil): This is what the using code should be talking to eventually.
	// Right now, this functions are just going to forward their argument, but 
	// the frontend can und should eventually be extended to include key- 
	// and bucket-based sorting. Maybe, instead of using an interface for 
	// the context, the backend funtions can be implemented seperately and 
	// instead just use callbacks to hook up to the frontend.
	//
	// Renderer should have its own memory area from which it allocates commands.
	// It can then hand the user a reference to the command so he can fill it.
	// On submit, the allocated commands can be sorted, executed and deleted.
	class Renderer
	{
	public:
		template <class Handle>
		void checkBufferValid(Handle handle)
		{
			if (!handle.isValid())
			{
				Logger::error("Failed to create VertexBuffer");
			}
		}

		VertexBufferHandle createVertexBuffer(const VertexBufferFormat& format);
		IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data);
		ShaderHandle createShader(const char* source, Shader::Type shaderType);
		ProgramHandle createProgram(const Shader::List& shaders);
		void draw(const Commands::DrawIndexed& command);
		void submit();

	private:
		IRenderContext* m_pContext;
	};
}