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
			UpArrow,
			DownArrow,
			LeftArrow,
			RightArrow,
			NumValues,
		};
		
		enum Modifier
		{
			Shift = 1 << 0,
			Ctrl  = 1 << 1,
			Alt   = 1 << 2,
			NumModifiers = 0
		};

		struct State
		{
			Input::Action m_action;
			int m_modifiers;
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

		Key::State m_keyStates[Key::Value::NumValues];
		MouseState m_mouseState;
	};
}