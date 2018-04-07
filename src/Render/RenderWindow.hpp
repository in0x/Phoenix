#pragma once

#include <stdint.h>
#include <Memory/CircularBuffer.hpp>

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

	struct MouseState
	{	
		float m_x;
		float m_y;
		bool m_leftDown; //temp
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
		MouseState m_mouseState;
	};
}