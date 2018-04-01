#pragma once

#include <vector>

namespace Phoenix
{
	// TODO: Ensure alignment of first alloc per chunk. 
	class ChunkArrayBase
	{
	public:
		ChunkArrayBase(size_t elemSizeBytes, size_t chunkSizeBytes)
			: m_sizePerAllocBytes(elemSizeBytes)
			, m_chunkSizeBytes(chunkSizeBytes)
			, m_nextAllocIdx(0)
			, m_elemsPerChunk(chunkSizeBytes / elemSizeBytes)
		{
		}

		ChunkArrayBase(size_t elemSizeBytes, size_t chunkSizeBytes, size_t initialCapacity)
			: m_sizePerAllocBytes(elemSizeBytes)
			, m_chunkSizeBytes(chunkSizeBytes)
			, m_nextAllocIdx(0)
			, m_elemsPerChunk(chunkSizeBytes / elemSizeBytes)
		{
			expandTo(initialCapacity);
		}

		virtual ~ChunkArrayBase()
		{
			for (char* chunk : m_chunks)
			{
				operator delete(chunk);
			}
		}

		size_t capacity() const
		{
			size_t c = m_elemsPerChunk * m_chunks.size();
			return c;
		}

		size_t size() const
		{
			size_t s = (m_nextAllocIdx / m_elemsPerChunk) * m_elemsPerChunk + (m_nextAllocIdx % m_elemsPerChunk);
			return s;
		}

		void* at(size_t idx)
		{
			assert(idx <= m_nextAllocIdx);
			return m_chunks[idx / m_elemsPerChunk] + (idx % m_elemsPerChunk) * m_sizePerAllocBytes;
		}

		void* alloc()
		{
			size_t next = m_nextAllocIdx++;
			expandTo(next);
			return at(next);
		}

		void swapAndPop(size_t idx)
		{
			void* toRemove = at(idx);
			void* last = at(m_nextAllocIdx--);

			memcpy(toRemove, last, m_sizePerAllocBytes);
			memset(last, 0, m_sizePerAllocBytes);
		}

	private:
		void expandTo(size_t numElements)
		{
			while (capacity() <= numElements)
			{
				m_chunks.push_back(static_cast<char*>(operator new(m_chunkSizeBytes)));
			}
		}

		std::vector<char*> m_chunks;
		size_t m_sizePerAllocBytes;
		size_t m_chunkSizeBytes;
		size_t m_nextAllocIdx;
		size_t m_elemsPerChunk;
	};

	template <typename T>
	class ChunkArrayIterator;

	template<typename T>
	class ChunkArray : public ChunkArrayBase
	{
	public:

		class ChunkArray(size_t elementsPerChunk)
			: ChunkArrayBase(sizeof(T), elementsPerChunk * sizeof(T))
		{}

		class ChunkArray(size_t elementsPerChunk, size_t initialCapacity)
			: ChunkArrayBase(sizeof(T), elementsPerChunk * sizeof(T), initialCapacity)
		{}

		~ChunkArray()
		{
			size_t numAlloced = size();
			for (size_t i = 0; i < numAlloced; ++i)
			{
				operator[](i).~T();
			}
		}

		T& operator[](size_t idx)
		{
			return *static_cast<T*>(at(idx));
		}

		const T& operator[](size_t idx) const
		{
			return *static_cast<const T*>(at(idx));
		}

		template <typename ...CtorArgs>
		T* add(CtorArgs... ctorArgs)
		{
			return new (alloc()) T(ctorArgs...);
		}

		size_t begin() const 
		{
			return 0;
		}

		size_t end() const
		{
			return size();
		}
	};

	template <typename T>
	class ChunkArrayIterator
	{
	public:
		ChunkArrayIterator(ChunkArray<T>* arr)
			: m_arr(arr)
		{
		}

		bool operator==(const ChunkArrayIterator& other) const
		{
			return m_current == other.m_current;
		}

		bool operator!=(const ChunkArrayIterator& other) const
		{
			return !(*this == other);
		};

		bool operator==(size_t otherIdx) const
		{
			return m_current == otherIdx;
		}

		bool operator!=(size_t otherIdx) const
		{
			return !(*this == otherIdx);
		}

		void operator++()
		{
			m_current++;
		}

		T& operator*()
		{
			return m_arr->operator[](m_current);
		}

		const T& operator*() const
		{
			return m_arr->operator[](m_current);
		}

		ChunkArrayIterator begin();

		ChunkArrayIterator end();

	protected:
		ChunkArrayIterator(ChunkArray<T>* arr, size_t startIndex)
			: m_arr(arr)
			, m_current(startIndex)
		{
		}

		ChunkArray<T>* m_arr;

	private:
		size_t m_current;
	};

	template <typename T>
	ChunkArrayIterator<T> ChunkArrayIterator<T>::begin()
	{
		return ChunkArrayIterator(m_arr, 0);
	}

	template <typename T>
	ChunkArrayIterator<T> ChunkArrayIterator<T>::end()
	{
		return ChunkArrayIterator(m_arr, m_arr->size());
	}
}