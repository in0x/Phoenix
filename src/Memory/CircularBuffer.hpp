#pragma once

#include <stdint.h>

namespace Phoenix
{
	template <typename T, size_t bufferSize>
	class CircularBuffer
	{
	public:
		CircularBuffer()
			: m_head(0)
			, m_tail(0)
			, m_bEmpty(true)
		{}

		void clear()
		{
			m_head = m_tail;
			m_bEmpty = true;
		}

		bool isEmpty()
		{
			return m_bEmpty;
		}

		size_t size() const
		{
			return (m_head > m_tail) ? (m_head - m_tail) : (bufferSize - m_tail) + m_head;
		}

		size_t capacity() const
		{
			return bufferSize;
		}

		void add(const T& item)
		{
			if (m_head == m_tail)
			{
				m_tail = (m_tail + 1) % bufferSize;
			}
			
			m_data[m_head] = item;
			m_head = (m_head + 1) % bufferSize;
		
			m_bEmpty = false;
		}

		T* get()
		{
			if (m_bEmpty)
			{
				return nullptr;
			}

			T* pItem = &m_data[m_tail];
			m_tail = (m_tail + 1) % bufferSize;

			m_bEmpty = (m_tail == m_head);

			return pItem;
		}

		T* data()
		{
			return m_data;
		}

	private:
		T m_data[bufferSize];
		size_t m_head;
		size_t m_tail;
		bool m_bEmpty;
	};
}