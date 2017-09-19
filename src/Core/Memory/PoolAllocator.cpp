#pragma once

#include "PoolAllocator.hpp"

namespace Phoenix
{
	PoolAllocator::PoolAllocator() {}

	void PoolAllocator::create(size_t blockSize, size_t numBlocks, size_t maxAlignment)
	{
		assert(maxAlignment >= 1);
		assert((maxAlignment & (maxAlignment - 1)) == 0); // power of 2

		m_blockSize = blockSize;
		m_alignment = maxAlignment;

		size_t required = blockSize * numBlocks + maxAlignment;

		m_start = reinterpret_cast<char*>(operator new(required));
		m_end = m_start + required;

		m_freeList = FreeList(m_start, m_end, blockSize, numBlocks, maxAlignment);
	}

	PoolAllocator::~PoolAllocator()
	{
		clear();
		operator delete(m_start);
	}

	void* PoolAllocator::allocate()
	{
		/*void* memory = m_freeList.allocate();
		assert(memory != nullptr):
		return memory;*/
		return m_freeList.allocate();
	}

	void PoolAllocator::free(void* memory)
	{
		char* blockAddr = reinterpret_cast<char*>(memory);
		assert((m_start <= blockAddr && m_end >= blockAddr));

		m_freeList.free(memory);
	}

	void PoolAllocator::clear()
	{
		memset(m_start, 0, m_end - m_start);
	}
}