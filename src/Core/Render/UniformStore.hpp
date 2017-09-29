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

	Really should get a memory allocator for uniform data

	*/

	struct UniformInfo
	{
		//char name[RenderConstants::c_maxUniformNameLenght]; // Storing these hashed makes much more sense
		Hash_t nameHash;
		Uniform::Type type;
		void* data;
		size_t size;
	};

	class UniformStore
	{
	public:
		explicit UniformStore(size_t count);

		const UniformInfo& create(UniformHandle handle, Uniform::Type type, const char* name, const void* data, size_t size);

		const UniformInfo& get(UniformHandle handle);

		void update(UniformHandle handle, const void* data, size_t size);

		void destroy(UniformHandle handle);

	private:
		PoolAllocator m_memory;
		std::vector<UniformInfo> m_uniforms;
	};

}