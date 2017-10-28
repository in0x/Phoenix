#pragma once

#include "RenderDefinitions.hpp"

#include "../Memory/PoolAllocator.hpp"

#include <vector>

namespace Phoenix
{
	/*
	Uniforms:
	When creating a program, store a map of all active uniforms and their location
	Store a uniform, its name, its datatype and its data in a seperate frontend cache

	When drawing, for all submitted uniforms, if not already uploaded, check if the uniform
	exists in the programs uniform map and if yes, upload the data

	We use two caches
	One that gets stored when creating a uniform using the frontend
	And one for each program that is used for a series of drawcalls (lets us temporarily check if we have already updated it)
	*/

	struct UniformInfo
	{
		//char name[RenderConstants::c_maxUniformNameLenght]; 
		Hash nameHash;
		EUniform::Type type;
		void* data;
		size_t size;
	};

	class UniformStore
	{
	public:
		explicit UniformStore(size_t count);

		const UniformInfo& create(size_t id, EUniform::Type type, const char* name, const void* data, size_t size);

		const UniformInfo& get(size_t id);

		void update(size_t id, const void* data, size_t size);

		void destroy(size_t id);

	private:
		PoolAllocator m_memory;
		std::vector<UniformInfo> m_uniforms;
	};

}