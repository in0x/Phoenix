#pragma once

#include "RIOpenGL.hpp"
#include "RIDeviceOpenGL.hpp"
#include "RIContextOpenGL.hpp"
#include "RIOpenGLResourceStore.hpp"

#include "../RenderWindow.hpp"
#include "../../glfw/glfw3.h"
#include "../../glew/glew.h"
#include "../../Logger.hpp"
#include <vector>

namespace Phoenix
{
	class GlRenderWindow : public RenderWindow
	{
	public:
		GlRenderWindow()
			: m_glfwWindow(nullptr)
		{
		}
		
		bool init(const WindowConfig& config)
		{
			m_glfwWindow = glfwCreateWindow(config.width, config.height, config.title, nullptr, nullptr);
			return (nullptr != m_glfwWindow);
		}

		~GlRenderWindow()
		{
			glfwDestroyWindow(m_glfwWindow);
		}

		virtual bool wantsToClose() override
		{
			return glfwWindowShouldClose(m_glfwWindow);
		}

		GLFWwindow* m_glfwWindow;
	};

	class RIOpenGL::Impl
	{
	public:
		Impl()
			: resources()
			, device(&resources)
			, context(&resources)
		{
		}

		RIOpenGLResourceStore resources;
		RIDeviceOpenGL device;
		RIContextOpenGL context;
		std::vector<GlRenderWindow> windows;
	};

	RIOpenGL::RIOpenGL()
	{
		m_pImpl = new RIOpenGL::Impl;
	}
	
	RIOpenGL::~RIOpenGL()
	{
		delete m_pImpl;
	}

	RenderWindow* RIOpenGL::createWindow(const WindowConfig& config)
	{
		m_pImpl->windows.push_back(GlRenderWindow());
		GlRenderWindow& window = m_pImpl->windows.back();

		if (!window.init(config))
		{
			Logger::errorf("Failed to create window with title %s", config.title);
			assert(false);
			return nullptr;
		}

		return &m_pImpl->windows.back();
	}

	void RIOpenGL::setWindowToRenderTo(RenderWindow* window)
	{
		GlRenderWindow* glWindow = static_cast<GlRenderWindow*>(window);
		glfwMakeContextCurrent(glWindow->m_glfwWindow);
	}

	void RIOpenGL::swapBufferToWindow(RenderWindow* window)
	{
		GlRenderWindow* glWindow = static_cast<GlRenderWindow*>(window);
		glfwSwapBuffers(glWindow->m_glfwWindow);
	}

	bool RIOpenGL::init()
	{
		if (!glfwInit())
		{
			Logger::error("Failed to initalize GLFW");
			assert(false);
			return false;
		}

		int windowWidth = 800;
		int windowHeight = 600;

		glfwWindowHint(GLFW_VISIBLE, 0);
		GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "BasicContext", nullptr, nullptr);
		
		if (!window)
		{
			Logger::error("Failed to create initial window");
			assert(false);
			return false;
		}

		glfwMakeContextCurrent(window);

		if (glewInit())
		{
			Logger::error("Failed to initialize GLEW");
			assert(false);
			return false;
		}

		glfwMakeContextCurrent(nullptr);
		glfwDestroyWindow(window);
		glfwDefaultWindowHints();

		return true;
	}

	IRIDevice* RIOpenGL::getRenderDevice()
	{
		return &m_pImpl->device;
	}
	
	IRIContext* RIOpenGL::getRenderContex()
	{
		return &m_pImpl->context;
	}

	namespace Platform
	{
		void pollEvents()
		{
			glfwPollEvents(); 
		}
	}
}
