#pragma once

#include <cstddef>

namespace Phoenix
{
	class StackAllocator
	{
	public:
		StackAllocator();
		
		void create(size_t size);

		~StackAllocator();

		StackAllocator& operator=(const StackAllocator&) = delete;

		StackAllocator(const StackAllocator&) = delete;

		StackAllocator(const StackAllocator&&) = delete;

		void* allocate(size_t size, size_t alignment = alignof(std::max_align_t));
	
		void free(void* memory);
		
		void clear();
	
	private:		
		char* m_start;
		char* m_end;
		char* m_top;
	};
}