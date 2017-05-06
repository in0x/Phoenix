#include <cassert>

#include "Tests/MathTests.hpp"
#include "Core/obj.hpp"

#include "gl3w/gl3w.h"
#include "GLFW/glfw3.h"

#include "Core/Transform.h"

char* getCMDOption(char** start, char** end, const std::string& option)
{
	char** iter = std::find(start, end, option);

	if (iter != end && ++iter != end)
	{
		return *iter;
	}

	return nullptr;
}

int main(int argc, char** argv)
{
	using namespace Phoenix;

	Logger::LogOut::get().logToConsole = true;
	Logger::LogOut::get().logToFile = false;

	Tests::RunMathTests();

	std::unique_ptr<Mesh> fox = parseOBJ("Fox/", "RedFox.obj");
	assert(fox != nullptr);

	if (!glfwInit())
	{
		printf("Failed to init GLFW\n");
	}
	else
	{
		printf("Inited GLFW\n");
	}

	int windowWidth = 1280;
	int windowHeight = 720;

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "DOS", nullptr, nullptr);
	if (!window)
	{
		printf("Failed to open window\n");
	}

	glfwMakeContextCurrent(window);

	if (gl3wInit())
	{
		printf("Failed to init glew");
	}

	while (!glfwWindowShouldClose(window))
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}