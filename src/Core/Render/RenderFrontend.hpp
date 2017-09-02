#pragma once

#include "RenderDefinitions.hpp"

namespace Phoenix
{
	// Splitup in frontend that handles all the shared stuff, such as creating handles, caching uniforms etc.
	// IRenderContext is backend which is used via commands. Allows us to pull alloc functions out RenderContext
	// where they dont belong.

	class IRenderBackend;

	namespace RenderFrontend
	{
		void init(RenderInit* renderInit);
		void submit();
		void swapBuffers();
		void exit();

		void tempSetProgram(ProgramHandle handle);
		IRenderBackend* getBackend();

		VertexBufferHandle createVertexBuffer(const VertexBufferFormat& format);
		
		IndexBufferHandle createIndexBuffer(size_t size, uint32_t count, const void* data);
		
		ShaderHandle createShader(const char* source, size_t strlen, Shader::Type shaderType);
		
		ProgramHandle createProgram(const Shader::List& shaders);

		//void createUniform(UniformHandle handle, const char* name, Uniform::Type type, const void* data);
	};

	// The user shouldn't have to work with commands at the game logic level,
	// i.e. ForwardRenderer takes in Renderables and produces commands for them.

	class ForwardRenderer
	{

	};
}