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

	struct Input
	{
		enum Action
		{
			Release,
			Press,
			Repeat,
			NumActions
		};
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
		
		enum Modifier
		{
			Shift = 1 << 0,
			Ctrl  = 1 << 1,
			Alt   = 1 << 2,
			NumModifiers = 0
		};

		struct Event
		{
			Event()
				: m_value(NumValues)
				, m_action(Input::NumActions)
				, m_modifiers(0)
			{}

			Value m_value;
			Input::Action m_action;
			int m_modifiers;

			bool hasModifier(Modifier mod)
			{
				return (m_modifiers & mod) != 0;
			}
		};
	};

	struct MouseState
	{		
		enum Button
		{
			Left,
			Right,
			Middle,
			Mouse4,
			Mouse5,
			NumButtons
		};

		float m_x;
		float m_y;

		Input::Action m_buttonStates[NumButtons];
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