#pragma once

#include "MemUtil.hpp"
#include "FreeList.hpp"

namespace Phoenix
{
	class PoolAllocator
	{
	public:
		PoolAllocator(size_t blockSize, size_t numBlocks, Alignment maxAlignment);

		~PoolAllocator();

		PoolAllocator& operator=(const PoolAllocator&) = delete;

		PoolAllocator(const PoolAllocator&) = delete;

		PoolAllocator(const PoolAllocator&&) = delete;

		void* allocate(size_t size, Alignment alignment);
	
		void free(void* memory);
	
		void clear();
		
	private:
		size_t m_blockSize;
		Alignment m_alignment;
		char* m_start;
		char* m_end;
		FreeList m_freeList;
	};
}
