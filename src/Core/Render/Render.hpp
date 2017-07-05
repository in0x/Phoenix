#pragma once

#include "../OpenGL.hpp"
#include "../Logger.hpp"

namespace Phoenix
{
	/*
		This module should be:
		- Responsible for submitting draw calls to the underlying graphics API selected by the user.
		- Capable of supporting multiple graphics APIs without requiring changes in user facing code.
		  The user should not have to interact with any code specific to the graphics API.
		- Initially supporting OpenGL, but open for extension to other APIs such as D3D, Metal, Vulkan etc.
		- Stateless, in order to lighten the mental load usually introduced by stateful APIs such as OpenGL.
		  Two draw calls should therefore be interchangable in their order without changing the end result.
		  A draw call should carry all its required state with it, while not being affected by the state set
		  by other draw calls.
	*/

	/*
		- A game creates a Renderer, this might be a ForwardRenderer, a DeferredRenderer, a ClusteredRenderer etc
		- A Renderer has n CommandBuckets used for storing the commands it requires to perform the Render step.
		  A CommandBucket has a variably sized key, used to sort commands. Certain bits in the key represent 
		  criteria to sort by, such as depth, material, mesh etc.
		  A DeferredRenderer might have a GBuffer Bucket, a Lights Bucket, a Normal Buckets etc.
		- A CommandBucket stores Commands. Commands represent actions that can be performed by the rendering backend,
		  such as drawing primitives, creating a buffer, etc.
		- After sorting the commands, the bucket can be flushed to submit the commands to the backend. 
		- A backend represents a rendering context, such as a hardware-accelerated API such as D3D or GL or even a software renderer.
		  A backend performs actions specific to that rendering context such as executing commands or allocating resources.
	*/

	/*
		Research:
		https://blog.molecular-matters.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1/
		http://www.goatientertainment.com/downloads/Designing%20a%20Modern%20GPU%20Interface.pptx
		https://www.gamedev.net/topic/684820-drawitems-for-temporary-rendering/
		https://www.gamedev.net/topic/604899-frostbite-rendering-architecture-question/
		https://github.com/bkaradzic/bgfx
		https://github.com/bkaradzic/bgfx/blob/30b6d07e292ba7803210db823968fd55e2f73937/src/renderer_gl.cpp
		https://github.com/bkaradzic/bgfx/blob/master/include/bgfx/bgfx.h
		https://github.com/bkaradzic/bgfx/blob/master/examples/01-cubes/cubes.cpp
		https://www.gamedev.net/topic/630029-data-driven-renderer/
		http://gamedevs.org/uploads/flexible-rendering-multiple-platforms.pdf
		http://amd-dev.wpengine.netdna-cdn.com/wordpress/media/2012/10/Andersson-Tatarchuk-FrostbiteRenderingArchitecture(GDC07_AMD_Session).pdf
		https://www.kth.se/social/upload/5283df9bf276542df7e59cb0/RikardBlomberg_KTH20131112.pdf
		http://realtimecollisiondetection.net/blog/?p=86
		https://www.google.at/search?q=sort-based+draw+call+bucketing&oq=sort-based+draw+call+bucketing&aqs=chrome..69i57.2604j0j4&sourceid=chrome&ie=UTF-8
		https://www.reddit.com/r/gamedev/comments/67jc0u/rendering_architecture_multiple_apis/
	*/


	/*
		Draw Items defined all of the pipeline state except for the Depth/Stencil Target and Render Targets.
		Render Passes define these destination resources, plus the default and override state groups.

		RenderPass* pass = CreatePass( depth, color, defaults, override )
		StateGroup* stack[] = { material, mesh }
		DrawCommand command = { 3, TRIANGLES }
		DrawItem* draw = Compile( stack, command, pass )
		DrawItem* draws[] = { draw }
		Submit(pass, draws)

		To represent resources, use integer handles (8-16 bit) to save space compared to pointers.
		Use resource managers to load/create/store all resources on start-up. A higher level object can
		wrap that handle.
	*/

	//class Shader {}; // Problem: These should already be compiled into a program. So it should not be possible specify a shader pair that has not be compiled into a program/effect/whatever after loading, right?

	class ShaderProgram { /* Compiled shader / program / effect */ };

	class GlShaderProgram : public ShaderProgram
	{
	public:
		GLuint m_nativeHandle;
	};

	class VertexBuffer {};

	class IndexBuffer {};

	namespace Primitive
	{
		enum Type
		{
			Points,
			Lines,
			Triangles
		};
	};

	class StateGroup
	{
		/*
			Contains config settings for render pipeline
			- What blend function
			- What textures (How does this work? Does the engine define a max number of textures (i.e. 16)
			and a stategroup can bind 0 .. n textures?)
			- Raster setting (what exactly is this?)
			- The input layout (I don't know how this can be defined since a GLSL shader defines its own input layout.
			Does the engine predefine the shader input layout and every shader has to abide by that? But then what is the point
			of this setting? Maybe to define what is used?)
			- Depth
			- Stencil
			- What buffers are bound
		*/
	public:
		StateGroup()
			: vertices(nullptr)
			, indices(nullptr)
			, shaderProgram(nullptr)
		{
		}

		VertexBuffer* vertices;
		IndexBuffer* indices;
		ShaderProgram* shaderProgram;
	};

	//class StateGroupWriter
	//{
	//	StateGroup group;

	//	void begin()
	//	{
	//		// Set defaults here?
	//		// Shouldn't they be set by constructor?
	//	}
	//	
	//	void end()
	//	{
	//	}

	//	void addShaderProgram(ShaderProgram* vertexShader)
	//	{
	//	}

	//	void addVertexBuffer(VertexBuffer* vertexBuffer)
	//	{
	//	}

	//	void addIndexBuffer(IndexBuffer* indexBuffer)
	//	{
	//	}
	//};

	// On Renderable can have multiple stategroups,
	// i.e. one from the mesh, one from the material, one for defaults
	// Order decides overrides, hodgeman has the front most stategroup as the highest priority,
	// i.e. settings set by a stategroup are always overriden if a stategroup in front of it sets 
	// the same setting. 
	using StateGroupStack = StateGroup*;

	struct DrawCommand
	{
		// Basically what is given in GlDrawArrays: What draw type (lines, points, triangles, strips)
		// and how many
		Primitive::Type primtiveType;
		size_t count;
		size_t startIndex;
	};

	struct DrawItem
	{
		// Represents a draw which is compiled from a StateGroup stack and a DrawCommand
		StateGroupStack states;
		DrawCommand drawCommand;
	};

	StateGroup evalStateGroupStack(StateGroupStack stateStack)
	{
		
	}

	namespace Commands
	{
		struct DrawIndexed
		{
			DrawItem* drawItem;
		};
	}


	namespace GlBackend
	{
		void draw(Commands::DrawIndexed* command)
		{
			GLenum primtiveType = GL_TRIANGLES;

			DrawCommand& drawCmd = command->drawItem->drawCommand;

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
				Logger::Warning("PrimitiveType unsupported, reverting to default triangles");
				assert(false);
			}
			}

			GlShaderProgram* glProgram = static_cast<GlShaderProgram*>(command->drawItem->states->shaderProgram);
			glUseProgram(glProgram->m_nativeHandle);

			glDrawElements(primtiveType, drawCmd.count, GL_UNSIGNED_INT, (GLvoid*)(sizeof(unsigned int) * drawCmd.startIndex));
		}
	
	}

	/*
		RendererInit,
		RendererShutdownBegin,
		CreateVertexDecl,
		CreateIndexBuffer,
		CreateVertexBuffer,
		CreateShader,
		CreateProgram,
		CreateTexture,
		UpdateTexture,
		ResizeTexture,
		CreateFrameBuffer,
		CreateUniform,
		End,
		RendererShutdownEnd,
		DestroyVertexDecl,
		DestroyIndexBuffer,
		DestroyVertexBuffer,
		DestroyShader,
		DestroyProgram,
		DestroyTexture,
		DestroyFrameBuffer,
		DestroyUniform,
		ReadTexture,
		RequestScreenShot,
	*/

#include <limits>

#undef max // y tho


#define HANDLE(name, size) \
	struct name \
	{ \
		static const size invalidValue = std::numeric_limits<size>::max(); \
		size idx = invalidValue; \
		static bool isValid(name handle) { return handle.idx != invalidValue; } \
	}; \
			

	HANDLE(VertexBufferHandle, uint16_t)
	HANDLE(IndexBufferHandle, uint16_t)
	HANDLE(ShaderHandle, uint16_t)
	HANDLE(ProgramHandle, uint16_t)
	HANDLE(TextureHandle, uint16_t)
	HANDLE(FrameBufferHandle, uint16_t)

	class IRenderContext
	{
	public: // Each of these adds a 
		virtual void init() = 0;
		virtual VertexBufferHandle createVertexBuffer() = 0;
		virtual IndexBufferHandle createIndexBuffer() = 0;
		virtual ShaderHandle createShader() = 0;
		virtual ProgramHandle createProgram() = 0;
		virtual TextureHandle createTexture() = 0;
		virtual FrameBufferHandle createFrameBuffer() = 0;
	};

	class GlRenderContext : public IRenderContext
	{
		virtual void init() override
		{
			GLenum err = glewInit(); // I need to do this when a context is created / made current for the first time.
			if (err != GLEW_OK)
			{
				Logger::Error("Failed to initialize gl3w");
				//return -1;
			}
		}
	
		virtual VertexBufferHandle createVertexBuffer() override
		{
			VertexBufferHandle handle;
			VertexBufferHandle::isValid(handle);
		}

		virtual IndexBufferHandle createIndexBuffer() = 0;
		virtual ShaderHandle createShader() = 0;
		virtual ProgramHandle createProgram() = 0;
		virtual TextureHandle createTexture() = 0;
		virtual FrameBufferHandle createFrameBuffer() = 0;

	};
}