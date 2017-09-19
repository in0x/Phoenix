#include "StackAllocator.hpp"
#include "MemUtil.hpp"

#include <assert.h>
#include <string.h>

namespace Phoenix
{
	/*template<class U, class T>
	U union_cast(T obj)
	{
	union
	{
	T as_T;
	U as_U;
	};

	as_T = obj;
	return U(as_T);
	}

	int t = 0;
	long u = union_cast<long>(t);*/

	StackAllocator::StackAllocator()
	{}

	void StackAllocator::create(size_t size)
	{
		m_start = reinterpret_cast<char*>(operator new(size));
		m_end = m_start + size;
		m_top = m_start;
	}

	StackAllocator::~StackAllocator()
	{
		clear();
		operator delete(m_start);
	}

	void StackAllocator::clear()
	{
		memset(m_start, 0, m_end - m_start);
		m_top = m_start;
	}

	void* StackAllocator::allocate(size_t size, size_t alignment/* = alignof(std::max_align_t)*/)
	{
		assert(alignment >= 1);
		assert((alignment & (alignment - 1)) == 0); // power of 2

		size_t fullSize = size + alignment;

		assert((m_top + fullSize) < m_end);

		char* aligned = alignAddressAndStore(m_top, alignment);

		m_top += fullSize;

		return aligned;
	}

	void StackAllocator::free(void* memory)
	{
		assert(memory != nullptr);

		char* alignedMem = reinterpret_cast<char*>(memory);

		assert((m_start <= alignedMem && m_end >= alignedMem));

		uintptr_t alignedAddress = reinterpret_cast<uintptr_t>(memory);
		ptrdiff_t adjustment = static_cast<ptrdiff_t>(alignedMem[-1]);
		uintptr_t rawAddress = alignedAddress - adjustment;

		m_top = reinterpret_cast<char*>(rawAddress);

		ptrdiff_t diff = m_top - m_start;

		return;
	}
}