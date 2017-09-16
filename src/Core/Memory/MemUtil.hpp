#pragma once

#include <cstdint>
#include <cassert>

namespace Phoenix
{
	// Returns the aligned version of the address.
	inline char* alignAddress(char* raw, size_t alignment)
	{
		uintptr_t ptr = reinterpret_cast<uintptr_t>(raw);

		size_t mask = alignment - 1;
		uintptr_t misalignment = ptr & mask;
		ptrdiff_t adjust = alignment - misalignment;
		assert(adjust < 256); // min 1 byte extra

		char* aligned = reinterpret_cast<char*>(raw + adjust);
		
		return aligned;
	}

	// Aligns raw to alignment and stores the 
	// alignment right before the address.
	inline char* alignAddressAndStore(char* raw, size_t alignment)
	{
		uintptr_t ptr = reinterpret_cast<uintptr_t>(raw);

		size_t mask = alignment - 1;
		uintptr_t misalignment = ptr & mask;
		ptrdiff_t adjust = alignment - misalignment;
		assert(adjust < 256); // min 1 byte extra

		char* aligned = reinterpret_cast<char*>(raw + adjust);
		aligned[-1] = static_cast<char>(adjust);

		return aligned;
	}

}