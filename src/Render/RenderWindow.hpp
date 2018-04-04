#pragma once

#include <stdint.h>

namespace Phoenix
{
	struct WindowConfig
	{
		uint32_t width;
		uint32_t height;
		const char* title;
		bool bFullscreen;
	};

	struct Key
	{
		enum Value
		{
			A,
			D,
			S,
			W,
			NumValues,
		};
		
		enum Action
		{
			Press, 
			Repeat,
			Release,
			NumActions
		};
		
		enum Modifier
		{
			Shift = 1,
			Ctrl = 2,
			Alt = 4,
			NumModifiers = 0
		};

		struct Event
		{
			Event()
				: m_value(NumValues)
				, m_action(NumActions)
				, m_modifiers(0)
			{}

			Value m_value;
			Action m_action;
			int m_modifiers;

			bool hasModifier(Modifier mod)
			{
				return m_modifiers & mod;
			}
		};
	};

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

	class RenderWindow
	{
	public:
		RenderWindow() {}
		virtual ~RenderWindow() {}

		virtual bool wantsToClose() { return false; }

		enum
		{
			MAX_KEY_EVENTS = 128
		};

		CircularBuffer<Key::Event, MAX_KEY_EVENTS> m_keyEvents;
	};
}