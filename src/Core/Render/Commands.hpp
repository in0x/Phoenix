#pragma once

#include "RenderDefinitions.hpp"
#include "RIResourceHandles.h"

#include "../Math/Vec3.hpp"
#include "../Math/Vec4.hpp"
#include "../Math/Matrix3.hpp"
#include "../Math/Matrix4.hpp"

// NOTE(Phil): Directly interpreting commands with OpenGL/DX might be more flexible (instead of using the IRenderBackend interface)

namespace Phoenix
{
	// A Command is a struct that can be interpreted using a RenderContext
	// in order to perform a certain action using the GPU. The command can
	// contain any data required for it to be executed. A command must 
	// contain a static pointer to a funtion with the SubmitFptr signature -
	// this is the submit function. The submit function uses the command's 
	// data and the RenderContext to execute the action.
	
	struct RIDrawIndexedCommand
	{
		uint32_t start;
		uint32_t count;

		EPrimitive primitives;
		VertexBufferHandle vertexBuffer;
		IndexBufferHandle indexBuffer;
		//CStateGroup state;
	};

	struct RIDrawLinearCommand
	{
		uint32_t start;
		uint32_t count;

		EPrimitive primitives;
		VertexBufferHandle vertexBuffer;
		//CStateGroup state;
	};

	/*struct ClearTarget
	{
		SUBMITTABLE();

		RGBA color;
		EBuffer::Type toClear;
		RenderTargetHandle handle;
	};*/

	struct RISetUniformInt32Command
	{
		int32_t data;
		ProgramHandle usingProgram;
		UniformHandle uniform;
	};

	struct RISetUniformFloatCommand
	{
		float data;
		ProgramHandle usingProgram;
		UniformHandle uniform;
	};

	struct RISetUniformVec3Command
	{
		Vec3 data;
		ProgramHandle usingProgram;
		UniformHandle uniform;
	};

	struct RISetUniformVec4Command
	{
		Vec4 data;
		ProgramHandle usingProgram;
		UniformHandle uniform;
	};

	struct RISetUniformMatrix3Command
	{
		Matrix3 data;
		ProgramHandle usingProgram;
		UniformHandle uniform;
	};

	struct RISetUniformMatrix4Command
	{
		Matrix4 data;
		ProgramHandle usingProgram;
		UniformHandle uniform;
	};

	struct RIUploadTexture2DCommand
	{
		const void* data;
		uint32_t width;
		uint32_t height;
		Texture2DHandle handle;
	};

/*	struct UploadCubemap
	{
		SUBMITTABLE();
			
		CubemapData data;
		uint32_t width;
		uint32_t height;
		TextureHandle handle;
	};*/

}