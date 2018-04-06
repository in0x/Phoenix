#pragma once

namespace Phoenix
{
	template <typename T, size_t bufferSize>
	class CircularBuffer
	{
	public:
		CircularBuffer()
			: m_head(0)
			, m_tail(0)
		{}

		void clear()
		{
			m_head = m_tail;
		}

		bool isFull()
		{
			return ((m_head + 1) % bufferSize) == m_tail;
		}

		bool isEmpty()
		{
			return m_head == m_tail;
		}

		void add(const T& item)
		{
			m_data[m_head] = item;
			m_head = (m_head + 1) % bufferSize;

			if (m_head == m_tail)
			{
				m_tail = (m_tail + 1) % bufferSize;
			}
		}

		T* get()
		{
			if (isEmpty())
			{
				return nullptr;
			}

			T* pItem = &m_data[m_tail];
			m_tail = (m_tail + 1) % bufferSize;
			return pItem;
		}

	private:
		T m_data[bufferSize];
		size_t m_head;
		size_t m_tail;
	};
}