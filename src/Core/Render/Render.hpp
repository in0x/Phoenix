#pragma once

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
		Research:
		https://blog.molecular-matters.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1/
		http://www.goatientertainment.com/downloads/Designing%20a%20Modern%20GPU%20Interface.pptx
		https://www.gamedev.net/topic/684820-drawitems-for-temporary-rendering/
		https://www.gamedev.net/topic/604899-frostbite-rendering-architecture-question/
		https://github.com/bkaradzic/bgfx
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
	};

	// On Renderable can have multiple stategroups,
	// i.e. one from the mesh, one from the material, one for defaults
	// Order decides overrides, hodgeman has the front most stategroup as the highest priority,
	// i.e. settings set by a stategroup are always overriden if a stategroup in front of it sets 
	// the same setting. 
	using StateGroupStack = StateGroup[];

	class DrawCommand
	{
		// Basically what is given in GlDrawArrays: What draw type (lines, points, triangles, strips)
		// and how many
	};

	class DrawItem
	{
		// Represents a draw which is compiled from a StateGroup stack and a DrawCommand
	};

	class Shader;

	class Program;

	class VertexBuffer;
	
	class IndexBuffer;
}