#pragma once

#include "RIOpenGL.hpp"
#include "RIDeviceOpenGL.hpp"
#include "RIContextOpenGL.hpp"
#include "RIOpenGLResourceStore.hpp"

#include <Render/RenderWindow.hpp>
#include <ThirdParty/glfw/glfw3.h>
#include <ThirdParty/glew/glew.h>
#include <Core/Logger.hpp>

#include <vector>

namespace Phoenix
{
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void mouseMoveCallback(GLFWwindow* window, double x, double y);

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

			if (m_glfwWindow)
			{
				glfwSetKeyCallback(m_glfwWindow, &keyCallback);
				glfwSetCursorPosCallback(m_glfwWindow, &mouseMoveCallback);
			}

			return (nullptr != m_glfwWindow);
		}

		~GlRenderWindow()
		{
			// TODO(phil.welsch): Do we need to unsub from key cb when window is destroyed?
			glfwDestroyWindow(m_glfwWindow);
		}

		virtual bool wantsToClose() override
		{
			return glfwWindowShouldClose(m_glfwWindow) == GLFW_TRUE;
		}

		GLFWwindow* m_glfwWindow;
	};

	class RIOpenGLImpl
	{
	public:
		RIOpenGLImpl()
			: resources()
			, device(&resources)
			, context(&resources)
		{
		}

		~RIOpenGLImpl()
		{
			for (GlRenderWindow* window : m_pWindows)
			{
				delete window;
			}
		}

		RIOpenGLResourceStore resources;
		RIDeviceOpenGL device;
		RIContextOpenGL context;
		std::vector<GlRenderWindow*> m_pWindows;
	};

	RIOpenGLImpl* g_impl = nullptr;

	GlRenderWindow* windowFromGlfw(GLFWwindow* glfwWindow)
	{
		auto iter = std::find_if(g_impl->m_pWindows.begin(), g_impl->m_pWindows.end(),
			[&glfwWindow](GlRenderWindow* other)
		{
			return glfwWindow == other->m_glfwWindow;
		});

		if (iter == g_impl->m_pWindows.end())
		{
			return nullptr;
		}

		return *iter;
	}

	RenderWindow* RI::createWindow(const WindowConfig& config)
	{
		GlRenderWindow* glWindow = new GlRenderWindow;

		if (!glWindow->init(config))
		{
			delete glWindow;
			Logger::errorf("Failed to create window with title %s", config.title);
			assert(false);
			return nullptr;
		}

		g_impl->m_pWindows.push_back(glWindow);

		return glWindow;
	}

	void RI::destroyWindow(RenderWindow* window)
	{
		if (window == nullptr)
		{
			return;
		}

		GlRenderWindow* glWindow = static_cast<GlRenderWindow*>(window);

		auto iter = std::find_if(g_impl->m_pWindows.begin(), g_impl->m_pWindows.end(),
			[&glWindow](GlRenderWindow* other)
		{
			return glWindow->m_glfwWindow == other->m_glfwWindow;
		});

		if (iter == g_impl->m_pWindows.end())
		{
			return;
		}

		*iter = nullptr;

		delete window;
	}

	void RI::setWindowToRenderTo(RenderWindow* window)
	{
		GlRenderWindow* glWindow = static_cast<GlRenderWindow*>(window);
		glfwMakeContextCurrent(glWindow->m_glfwWindow);
	}

	void RI::swapBufferToWindow(RenderWindow* window)
	{
		GlRenderWindow* glWindow = static_cast<GlRenderWindow*>(window);
		glfwSwapBuffers(glWindow->m_glfwWindow);
	}

	bool RI::init()
	{
		if (g_impl)
		{
			return false;
		}

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
		
		if (nullptr == window)
		{
			Logger::error("Failed to create initial window");
			assert(false);
			return false;
		}

		glfwMakeContextCurrent(window);

		if (glewInit() != 0)
		{
			Logger::error("Failed to initialize GLEW");
			assert(false);
			return false;
		}

		const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
		const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		const char* glslVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

		Logger::log(version);
		Logger::log(vendor);
		Logger::log(renderer);
		Logger::log(glslVersion);
		
		GLint majorVersion = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);

		if (majorVersion < 4)
		{
			Logger::errorf("Phoenix currently assumes atleast GL Version 4. Detected Version: %d.", majorVersion);
			assert(false);
			return false;
		}

		glfwMakeContextCurrent(nullptr);
		glfwDestroyWindow(window);
		glfwDefaultWindowHints();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		g_impl = new RIOpenGLImpl;
		return true;
	}

	void RI::exit()
	{
		delete g_impl;
		glfwTerminate();
	}

	IRIDevice* RI::getRenderDevice()
	{
		return &g_impl->device;
	}
	
	IRIContext* RI::getRenderContex()
	{
		return &g_impl->context;
	}

	void keyTypeFromGlfw(int key, int scancode, Key::Event* outEvent)
	{

#define IMPL_KEY_SWITCH(glfw, value) \
		case glfw: \
		{ \
			outEvent->m_value = value; \
			return; \
		} \

		switch (key)
		{
			IMPL_KEY_SWITCH(GLFW_KEY_A, Key::A)
			IMPL_KEY_SWITCH(GLFW_KEY_D, Key::D)
			IMPL_KEY_SWITCH(GLFW_KEY_S, Key::S)
			IMPL_KEY_SWITCH(GLFW_KEY_W, Key::W)
			default:
			{
				Logger::warningf("Attempted to convert unhandled key value: %s", glfwGetKeyName(key, scancode));
				break;
			}
		}
	}

	void actionFromGlfw(int action, Key::Event* outEvent)
	{
		switch (action)
		{
		case GLFW_PRESS:
			outEvent->m_action = Key::Press;
			break;
		case GLFW_REPEAT:
			outEvent->m_action = Key::Repeat; 
			break;
		case GLFW_RELEASE:
			outEvent->m_action = Key::Release;
			break;
		default:
			assert(false);
			Logger::warningf("Attempted to convert unhandled action value");
			break;
		}
	}

	void modifierFromGlfw(int mods, Key::Event* outEvent)
	{
		if (mods & GLFW_MOD_SHIFT)
		{
			outEvent->m_modifiers |= Key::Shift;
		}

		if (mods & GLFW_MOD_CONTROL)
		{
			outEvent->m_modifiers |= Key::Ctrl;
		}
			
		if (mods & GLFW_MOD_ALT)
		{
			outEvent->m_modifiers |= Key::Alt;
		}
	}

	namespace Platform
	{
		void pollEvents()
		{
			glfwPollEvents(); 
		}
	}

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_UNKNOWN)
		{
			Logger::warning("Key event with unknow key recieved, discarding.");
			return;
		}
		
		GlRenderWindow* renderWindow = windowFromGlfw(window);

		Key::Event ev;

		// TODO(phil.welsch): Use scancode to detect specific keys (e.g. left vs right shift)
		keyTypeFromGlfw(key, scancode, &ev);
		actionFromGlfw(action, &ev);
		modifierFromGlfw(mods, &ev); 

		renderWindow->m_keyEvents.add(ev);
	}

	void mouseMoveCallback(GLFWwindow* window, double x, double y)
	{
		GlRenderWindow* renderWindow = windowFromGlfw(window);
		renderWindow->m_mouseState.m_x = x;
		renderWindow->m_mouseState.m_y = y;
	}
}
