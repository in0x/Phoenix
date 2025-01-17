#pragma once

#include "FreeList.hpp"
#include "MemUtil.hpp"

namespace Phoenix
{
	FreeList::FreeList()
		: m_next(nullptr)
	{}

	FreeList::FreeList(void* memStart, void* memEnd, size_t blockSize, size_t numBlocks, size_t alignment) 
	{
		assert(blockSize >= sizeof(void*));

		// Assign each block to point to the next free block.
		union
		{
			void* asVoid;
			char* asChar;
			FreeList* asFree;
		};

		asVoid = memStart;

		ptrdiff_t adjust = getAdjustment(asChar, alignment);
		asChar += adjust;
		m_next = asFree;

		FreeList* iter = m_next;
		for (size_t i = 1; i < numBlocks; ++i)
		{
			asChar += blockSize;
			iter->m_next = asFree;
			iter = asFree;
		}

		size_t diff = static_cast<char*>(memEnd) - reinterpret_cast<char*>(iter);
		assert(blockSize == diff);
		iter->m_next = nullptr;
	}

	void* FreeList::allocate()
	{
		if (nullptr == m_next)
		{
			return nullptr; // No free blocks left.
		}

		FreeList* head = m_next;
		m_next = head->m_next;
		return head;
	}

	void FreeList::free(void* block)
	{
		// NOTE(Phil): User should check if block is in 
		// memory bounds, e.g. PoolAllocator.
		FreeList* head = static_cast<FreeList*>(block);
		head->m_next = m_next;
		m_next = head;
	}
}