#include "MemoryTests.hpp"

#include "../Core/Memory/PoolAllocator.hpp"
#include "../Core/Memory/StackAllocator.hpp"

namespace Phoenix { namespace Tests
{
	void runMemoryTests()
	{
		void poolTest();
		void stackTest();
	}

	void poolTest()
	{
		PoolAllocator pool(32, 32, 2);
		
		void* blocks[32];
		blocks[0] = pool.allocate(32, 2);

		for (size_t i = 1; i < 32; ++i)
		{
			blocks[i] = pool.allocate(32, 2);
			ptrdiff_t diff = reinterpret_cast<char*>(blocks[i]) - reinterpret_cast<char*>(blocks[i - 1]);
			assert(32 >= diff);
		}

		assert(pool.allocate(32, 2) == nullptr);
		pool.free(blocks[16]);
		assert(pool.allocate(32, 2) != nullptr);
	}

	void stackTest()
	{
		StackAllocator stack(1024);
		
		for (size_t i = 0; i < 32; ++i)
		{
			stack.allocate(32, 2);
		}
	}
} }