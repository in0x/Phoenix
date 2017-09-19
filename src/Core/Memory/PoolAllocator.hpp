#pragma once

#include <assert.h>
#include <string.h>
#include "MemUtil.hpp"
#include "FreeList.hpp"

namespace Phoenix
{
	class PoolAllocator
	{
	public:
		PoolAllocator();

		void create(size_t blockSize, size_t numBlocks, size_t maxAlignment);
	
		~PoolAllocator();

		PoolAllocator& operator=(const PoolAllocator&) = delete;

		PoolAllocator(const PoolAllocator&) = delete;

		PoolAllocator(const PoolAllocator&&) = delete;

		// NOTE(Phil): Should possibly have size and alignment arguments
		// if we're gonna use memory policies later.
		void* allocate();
	
		void free(void* memory);
	
		void clear();
		
	private:
		size_t m_blockSize;
		size_t m_alignment;
		char* m_start;
		char* m_end;
		FreeList m_freeList;
	};
}
