#pragma once

#include <stdint.h>
#include <assert.h>
#include <cstddef>

namespace Phoenix
{
#define MEGABYTE(x) 1000000 * x
#define GIGABYTE(x) 1000000000 * x

	struct Megabyte
	{
		explicit Megabyte(size_t bytes)
			: size(1000000 * bytes)
		{
		}

		size_t size;
	};

	// TODO(Phil): ^ Which one is better? Struct should allow size_t conversion probably.

	inline ptrdiff_t getAdjustment(char* raw, size_t alignment)
	{
		uintptr_t ptr = reinterpret_cast<uintptr_t>(raw);

		size_t mask = (alignment - 1);
		uintptr_t misalignment = (ptr & mask);
		return alignment - misalignment;
	}

	// Returns the aligned version of the address.
	inline char* alignAddress(char* raw, size_t alignment)
	{
		ptrdiff_t adjust = getAdjustment(raw, alignment);
		char* aligned = reinterpret_cast<char*>(raw + adjust);
		
		return aligned;
	}

	// Aligns raw to alignment and stores the 
	// alignment right before the address.
	inline char* alignAddressAndStore(char* raw, size_t alignment)
	{
		ptrdiff_t adjust = getAdjustment(raw, alignment);
		assert(adjust < 256); // min 1 byte extra

		char* aligned = reinterpret_cast<char*>(raw + adjust);
		aligned[-1] = static_cast<char>(adjust);

		return aligned;
	}

	struct Tracker // TODO(Phil): Extend and use this to track allocations in all allocators
	{
		char* m_start;
		char* m_end;
		uint32_t m_numAllocs;
	};

	class Alignment
	{
	public:
		Alignment(size_t value)
			: m_value(value)
		{
			assert(value >= 1);
			assert((value & (value - 1)) == 0);	
		}

		bool fitsInto(const Alignment& larger) const
		{
			return larger.m_value >= m_value;
		}
		
		const size_t m_value;
	};

	inline Alignment platformMaxAlignment()
	{
		return{ alignof(std::max_align_t) };
	}

	template <class T, class Allocator>
	T* alloc(Allocator& allocator)
	{
		return new (allocator.allocate(sizeof(T), alignof(T))) T;
	}

	template <class T, class Allocator>
	T* alloc(Allocator& allocator, T& t)
	{
		return new (allocator.allocate(sizeof(T), alignof(T))) T(t);
	}

	template <class T, class Allocator>
	void dealloc(Allocator& allocator, T& t)
	{
		t.~T();
		allocator.free(&t);
	}
}