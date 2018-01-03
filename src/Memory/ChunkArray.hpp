#pragma once

namespace Phoenix
{
	// TODO: Ensure alignment of first alloc per chunk. 
	class ChunkArrayBase
	{
	public:
		ChunkArrayBase(size_t elemSizeBytes, size_t chunkSizeBytes)
			: m_sizePerAllocBytes(elemSizeBytes)
			, m_chunkSizeBytes(chunkSizeBytes)
			, m_lastAllocIdx(0)
			, m_elemsPerChunk(chunkSizeBytes / elemSizeBytes)
		{
		}

		ChunkArrayBase(size_t elemSizeBytes, size_t chunkSizeBytes, size_t initialCapacity)
			: m_sizePerAllocBytes(elemSizeBytes)
			, m_chunkSizeBytes(chunkSizeBytes)
			, m_lastAllocIdx(0)
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
			size_t s = (m_lastAllocIdx / m_elemsPerChunk) * m_elemsPerChunk + (m_lastAllocIdx % m_elemsPerChunk);
			return s;
		}

		void* at(size_t idx)
		{
			assert(idx <= m_lastAllocIdx);
			return m_chunks[idx / m_elemsPerChunk] + (idx % m_elemsPerChunk) * m_sizePerAllocBytes;
		}

		void* alloc()
		{
			size_t next = m_lastAllocIdx++;
			expandTo(next);
			return at(next);
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
		size_t m_lastAllocIdx;
		size_t m_elemsPerChunk;
	};

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
				operator[](i)->~T();
			}
		}

		T* operator[](size_t idx)
		{
			return static_cast<T*>(at(idx));
		}

		template <typename ...CtorArgs>
		T* add(CtorArgs... ctorArgs)
		{
			return new (alloc()) T(ctorArgs...);
		}
	};
}