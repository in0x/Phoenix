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

	StackAllocator::StackAllocator(size_t size)
	{
		m_start = static_cast<char*>(operator new(size));
		m_end = m_start + size;
		m_top = m_start;
	}

	StackAllocator::~StackAllocator()
	{
		clear();
		operator delete(m_start);
	}

	//StackAllocator::StackAllocator(const StackAllocator& other)
	//{
	//	size_t size = other.m_end - other.m_start;
	//	
	//	m_start = reinterpret_cast<char*>(operator new(size));
	//	memcpy(m_start, other.m_start, size);
	//	m_end = m_start + size;
	//	m_top = m_start;
	//}
	//
	//StackAllocator& StackAllocator::operator=(const StackAllocator& other)
	//{
	//	operator delete(m_start);
	//	
	//}

	void StackAllocator::clear()
	{
		memset(m_start, 0, m_end - m_start);
		m_top = m_start;
	}

	void* StackAllocator::allocate(size_t size, const Alignment& alignment)
	{
		size_t fullSize = size + alignment.m_value;

		assert((m_top + fullSize) < m_end);

		char* aligned = alignAddressAndStore(m_top, alignment.m_value);

		m_top += fullSize;

		return aligned;
	}

	void StackAllocator::free(void* memory)
	{
		assert(memory != nullptr);

		char* alignedMem = static_cast<char*>(memory);

		assert((m_start <= alignedMem && m_end >= alignedMem));

		uintptr_t alignedAddress = reinterpret_cast<uintptr_t>(memory);
		ptrdiff_t adjustment = static_cast<ptrdiff_t>(alignedMem[-1]);
		uintptr_t rawAddress = alignedAddress - adjustment;

		m_top = reinterpret_cast<char*>(rawAddress);

		return;
	}
}