#pragma once

#include <map>
#include <limits>
#include "RenderDefinitions.hpp"


namespace Phoenix
{
	// For binding uniforms, for each shader store all known uniform locations.
	// When updating a uniform, look up if the location is cached, else fetch it from
	// the program.

	/*class UniformCache
	{
		void init(IRenderBackend* rc);
		uint32_t getUniformLocation(ProgramHandle program, UniformHandle uniform);
		const uint32_t* getUniformLocationIfCached(ProgramHandle program, UniformHandle uniform);
	};*/

	struct CachedUniform
	{
		char name[RenderConstants::c_maxUniformNameLenght];
		Uniform::Type type;
		UniformHandle handle;
	};
	
	class UniformCache
	{
	public:
		void addUniform(ProgramHandle program, UniformHandle uniform, UniformInfo info)
		{
		
		}
		
		bool getUniform(ProgramHandle program, const char* name, CachedUniform* outInfo)
		{
		
		}
	};
}