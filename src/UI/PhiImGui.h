#pragma once

#include "ThirdParty/imgui/imgui.h"

namespace Phoenix
{
	struct UiInputConsume
	{
		bool m_bConsumedKeyboard;
		bool m_bConsumedMouse;
	};

	void initImGui(class RenderWindow* window);
	void startFrameImGui();
	void renderImGui();
	UiInputConsume filterUiConsumedInput();
	void exitImGui();
}