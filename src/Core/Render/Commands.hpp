#pragma once

#include "RenderDefinitions.hpp"

// NOTE(Phil): Directly interpreting commands with OpenGL/DX might be more flexible (instead of using the IRenderBackend interface)

namespace Phoenix
{
	// A Command is a struct that can be interpreted using a RenderContext
	// in order to perform a certain action using the GPU. The command can
	// contain any data required for it to be executed. A command must 
	// contain a static pointer to a funtion with the SubmitFptr signature -
	// this is the submit function. The submit function uses the command's 
	// data and the RenderContext to execute the action.
	namespace Commands
	{
		struct DrawIndexed
		{
			SUBMITTABLE();

			uint32_t start;
			uint32_t count;

			EPrimitive::Type primitives;
			VertexBufferHandle vertexBuffer;
			IndexBufferHandle indexBuffer;
			CStateGroup state;
		};

		struct DrawLinear
		{
			SUBMITTABLE();

			uint32_t start;
			uint32_t count;

			EPrimitive::Type primitives;
			VertexBufferHandle vertexBuffer;
			CStateGroup state;
		};

		struct ClearBuffer
		{
			SUBMITTABLE();

			RGBA color;
			EBuffer::Type toClear;
			FrameBufferHandle handle;
		};

		struct CreateVertexBuffer
		{
			SUBMITTABLE();

			VertexBufferFormat format;
			VertexBufferHandle handle;
		};

		struct CreateIndexBuffer
		{
			SUBMITTABLE();

			const void* data;
			size_t size;
			uint32_t count;
			IndexBufferHandle handle;
		};

		struct CreateShader
		{
			SUBMITTABLE();

			char* source;
			EShader::Type shaderType;
			ShaderHandle handle;
		};

		struct CreateProgram
		{
			SUBMITTABLE();

			EShader::List shaders;
			ProgramHandle handle;
		};

		struct SetUniform
		{
			SUBMITTABLE();

			const void* data;
			UniformHandle handle;
		};

		struct createTexture
		{
			SUBMITTABLE();

			ETexture::Description desc;
			char* name;
			ETexture::Format format;
			TextureHandle handle;
		};

		struct UploadTexture
		{
			SUBMITTABLE();

			const void* data;
			uint32_t width;
			uint32_t height;
			TextureHandle handle;
		};

		struct UploadCubemap
		{
			SUBMITTABLE();
			
			ETexture::CubemapData data;
			uint32_t width;
			uint32_t height;
			TextureHandle handle;
		};
	}
}