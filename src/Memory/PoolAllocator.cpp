#pragma once

#include "PoolAllocator.hpp"

#include <assert.h>
#include <string.h>

namespace Phoenix
{
	PoolAllocator::PoolAllocator(size_t blockSize, size_t numBlocks, Alignment maxAlignment)
		: m_blockSize(blockSize)
		, m_alignment(maxAlignment)
	{
		size_t required = blockSize * numBlocks + maxAlignment.m_value;

		m_start = static_cast<char*>(operator new(required));
		m_end = m_start + required;

		m_freeList = FreeList(m_start, m_end, blockSize, numBlocks, maxAlignment.m_value);
	}

	PoolAllocator::~PoolAllocator()
	{
		clear();
		operator delete(m_start);
	}

	void* PoolAllocator::allocate(size_t size, Alignment alignment)
	{
		assert(alignment.fitsInto(m_alignment) && size > 0);
		return m_freeList.allocate();
	}

	void PoolAllocator::free(void* memory)
	{
		char* blockAddr = static_cast<char*>(memory);
		assert((m_start <= blockAddr && m_end >= blockAddr));
		assert(((blockAddr - m_start) % m_alignment.m_value) == 0);

		m_freeList.free(memory);
	}

	void PoolAllocator::clear()
	{
		memset(m_start, 0, m_end - m_start);
	}
}