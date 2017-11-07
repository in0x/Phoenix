#pragma once

namespace Phoenix
{
	class FreeList
	{
	public:
		FreeList();

		// TODO(Phil): How do we handle block sizes < sizeof(void*) ?
		explicit FreeList(void* memStart, void* memEnd, size_t blockSize, size_t numBlocks, size_t alignment); 

		void* allocate();

		void free(void* block);

	private:
		FreeList* m_next;
	};
}