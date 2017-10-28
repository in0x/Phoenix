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

		void* allocate(size_t size, const Alignment& alignment);
	
		void free(void* memory);
		
		void clear();
	
	private:		
		StackAllocator(const StackAllocator& other) = delete;

		StackAllocator& operator=(const StackAllocator& other) = delete;

		char* m_start;
		char* m_end;
		char* m_top;
	};
}