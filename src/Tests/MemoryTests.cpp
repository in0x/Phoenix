#include "MemoryTests.hpp"

#include "../Core/Memory/PoolAllocator.hpp"
#include "../Core/Memory/StackAllocator.hpp"

namespace Phoenix { namespace Tests
{
	void runMemoryTests()
	{
		poolTest();
		stackTest();
	}

	void poolTest()
	{
		PoolAllocator pool(32, 32, 4);
		
		void* blocks[32];
		blocks[0] = pool.allocate(32, 4);

		for (size_t i = 1; i < 32; ++i)
		{
			blocks[i] = pool.allocate(32, 4);
			ptrdiff_t diff = reinterpret_cast<char*>(blocks[i]) - reinterpret_cast<char*>(blocks[i - 1]);
			assert(32 >= diff);
		}

		/*char* offPtr = static_cast<char*>(blocks[3]);
		offPtr += 3;
		void* offVoid = reinterpret_cast<void*>(offPtr);
		pool.free(offVoid);*/

		assert(pool.allocate(32, 4) == nullptr);
		pool.free(blocks[16]);
		assert(pool.allocate(32, 4) != nullptr);
	}

	void stackTest()
	{
		// TODO(Phil): Find out why this asserts (prob alignment larger).
	/*	StackAllocator stack(1024);
		
		for (size_t i = 0; i < 32; ++i)
		{
			stack.allocate(32, 2);
		}*/
	}
} }