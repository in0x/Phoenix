#pragma once

#include <map>
#include <limits>
#include "RenderDefinitions.hpp"


namespace Phoenix
{
	// For binding uniforms, for each shader store all known uniform locations.
	// When updating a uniform, look up if the location is cached, else fetch it from
	// the program.

	class UniformCache
	{
		void init(IRenderBackend* rc);
		uint32_t getUniformLocation(ProgramHandle program, UniformHandle uniform);
		const uint32_t* getUniformLocationIfCached(ProgramHandle program, UniformHandle uniform);
	};
}