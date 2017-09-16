#pragma once

#include <cassert>
#include "MemUtil.hpp"

namespace Phoenix
{
	class FreeList
	{
	public:
		FreeList()
			: m_next(nullptr)
		{}

		explicit FreeList(void* memStart, void* memEnd, size_t blockSize, size_t numBlocks, size_t alignment) // if block size atleast 32 bit, store pointers, otherwise store indices
		{
			assert(blockSize >= sizeof(void*));
			
			{
				char* start = static_cast<char*>(memStart);
				char* end = static_cast<char*>(memEnd);
				ptrdiff_t size = end - start;
				assert(size >= ((blockSize * numBlocks) + numBlocks * alignment));
			}

			// Assign each block to point to the next free block.
			union
			{
				void* asVoid;
				char* asChar;
				FreeList* asFree;
			};

			asVoid = memStart;
			m_next = asFree; 
			asChar += blockSize + alignment; 

			// Aligned
			FreeList* iter = m_next;
			for (size_t i = 1; i < numBlocks; ++i)
			{
				iter->m_next = asFree/*reinterpret_cast<FreeList*>(alignAddress(asChar, alignment))*/;
				iter = asFree;
				asChar += blockSize + alignment;
			}

			iter->m_next = nullptr;
		}

		void* allocate()
		{
			if (nullptr == m_next)
			{
				return nullptr; // No free blocks left.
			}

			FreeList* head = m_next;
			m_next = head->m_next;
			return head;
		}

		void free(void* block)
		{
			// Assume that user already checked block is in memory bounds.
			FreeList* head = static_cast<FreeList*>(block);
			head->m_next = m_next;
			m_next = head;
		}

	private:
		FreeList* m_next;
	};

	class PoolAllocator
	{
	public:
		explicit PoolAllocator(size_t blockSize, size_t numBlocks, size_t maxAlignment)
		{
			assert(maxAlignment >= 1);
			assert((maxAlignment & (maxAlignment - 1)) == 0); // power of 2

			size_t required =  blockSize * numBlocks + numBlocks * maxAlignment;

			m_start = reinterpret_cast<char*>(operator new(required));
			m_end = m_start + required;

			m_freeList = FreeList(m_start, m_end, blockSize, numBlocks, maxAlignment);
		}

		~PoolAllocator()
		{
			clear();
			operator delete(m_start);
		}

		PoolAllocator& operator=(const PoolAllocator&) = delete;

		PoolAllocator(const PoolAllocator&) = delete;

		PoolAllocator(const PoolAllocator&&) = delete;

		// NOTE(Phil): Should possibly have size and alignment arguments
		// if we're gonna use memory policies later.
		void* allocate() 
		{
			/*void* memory = m_freeList.allocate();
			assert(memory != nullptr):
			return memory;*/
			return m_freeList.allocate();
		}
		
		void free(void* memory)
		{
			char* blockAddr = reinterpret_cast<char*>(memory);
			assert((m_start <= blockAddr && m_end >= blockAddr)); 

			m_freeList.free(memory);
		}

		void clear()
		{
			memset(m_start, 0, m_end - m_start);
		}
	
	private:
		char* m_start;
		char* m_end;
		FreeList m_freeList;
	};

}
