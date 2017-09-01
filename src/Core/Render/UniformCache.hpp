#pragma once

#include <map>
#include <limits>
#include "Render.hpp"

namespace Phoenix
{
	// For binding uniforms, for each shader store all known uniform locations.
	// When updating a uniform, look up if the location is cached, else fetch it from
	// the program.

	namespace uniformCache
	{
		void init(IRenderContext* rc);
		uint32_t getUniformLocation(ProgramHandle program, UniformHandle uniform);
		const uint32_t* getUniformLocationIfCached(ProgramHandle program, UniformHandle uniform);
	}
}