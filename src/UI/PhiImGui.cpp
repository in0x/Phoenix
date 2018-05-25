#include "PhiImGui.h"
#include "ThirdParty/imgui/glfwExample/imgui_impl_glfw_gl3.h"

#include <Render/RenderWindow.hpp>

namespace Phoenix
{
	void initImGui(RenderWindow* window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window->getApiHandle(), true);
		ImGui::StyleColorsDark();
	}

	void startFrameImGui()
	{
		ImGui_ImplGlfwGL3_NewFrame(); 
	}
	
	void renderImGui()
	{
		ImGui_ImplGlfwGL3_RenderDrawData();
	}

	UiInputConsume filterUiConsumedInput()
	{
		ImGuiIO& io = ImGui::GetIO();

		UiInputConsume consume;
		consume.m_bConsumedKeyboard = io.WantCaptureKeyboard;
		consume.m_bConsumedMouse = io.WantCaptureMouse;
		return consume;
	}

	void exitImGui()
	{
		ImGui_ImplGlfwGL3_Shutdown();
		ImGui::DestroyContext();
	}
}