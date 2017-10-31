#pragma once

namespace Phoenix
{
	template <size_t size>
	class StaticStackAllocator
	{
		void* allocate(size_t size, const Alignment& alignment)

		void free(void* memory);

		void clear();

	private:
		uint8_t m_bytes[size];

		StackAllocator(const StackAllocator& other) = delete;

		StackAllocator& operator=(const StackAllocator& other) = delete;

		char* m_start;
		char* m_end;
		char* m_top;
	};
}