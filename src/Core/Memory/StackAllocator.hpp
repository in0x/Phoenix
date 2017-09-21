#pragma once

#include "MemUtil.hpp"

namespace Phoenix
{
	class Alignment;

	class StackAllocator
	{
	public:
		StackAllocator(size_t size);
		
		~StackAllocator();

		StackAllocator& operator=(const StackAllocator&) = delete;

		StackAllocator(const StackAllocator&) = delete;

		StackAllocator(const StackAllocator&&) = delete;

		void* allocate(size_t size, const Alignment& alignment);
	
		void free(void* memory);
		
		void clear();
	
	private:		
		char* m_start;
		char* m_end;
		char* m_top;
	};
}