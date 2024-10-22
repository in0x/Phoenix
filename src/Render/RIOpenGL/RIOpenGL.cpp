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
#include <algorithm>

namespace Phoenix
{
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	class GlRenderWindow : public RenderWindow
	{
	public:
		GlRenderWindow()
			: m_glfwWindow(nullptr)
		{
		}
		
		bool init(const WindowConfig& config)
		{
			m_glfwWindow = glfwCreateWindow(config.width, 
											config.height, 
											config.title,	
											config.bFullscreen ? glfwGetPrimaryMonitor() : nullptr,
											nullptr);

			if (m_glfwWindow)
			{
				glfwSetKeyCallback(m_glfwWindow, &keyCallback);
				glfwSetMouseButtonCallback(m_glfwWindow, &mouseButtonCallback);
			}

			return (nullptr != m_glfwWindow);
		}

		virtual void* getApiHandle()
		{
			return static_cast<void*>(m_glfwWindow);
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

		g_impl = new RIOpenGLImpl;

		glfwMakeContextCurrent(nullptr);
		glfwDestroyWindow(window);
		glfwDefaultWindowHints();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
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

	Key::Value keyTypeFromGlfw(int key, int scancode)
	{

#define IMPL_KEY_SWITCH(glfw, value) \
		case glfw: \
		{ \
			return value; \
		} \

		switch (key)
		{
			IMPL_KEY_SWITCH(GLFW_KEY_A,		Key::A)
			IMPL_KEY_SWITCH(GLFW_KEY_D,		Key::D)
			IMPL_KEY_SWITCH(GLFW_KEY_S,		Key::S)
			IMPL_KEY_SWITCH(GLFW_KEY_W,		Key::W)
			IMPL_KEY_SWITCH(GLFW_KEY_UP,	Key::UpArrow)
			IMPL_KEY_SWITCH(GLFW_KEY_DOWN,  Key::DownArrow)
			IMPL_KEY_SWITCH(GLFW_KEY_LEFT,  Key::LeftArrow)
			IMPL_KEY_SWITCH(GLFW_KEY_RIGHT, Key::RightArrow)
			default:
			{
				Logger::warningf("Attempted to convert unhandled key value: %s", glfwGetKeyName(key, scancode));
				return Key::NumValues;
			}
		}
	}

	Input::Action actionFromGlfw(int action)
	{
		switch (action)
		{
		case GLFW_PRESS:
			return Input::Press;
		case GLFW_REPEAT:
			return Input::Repeat;
		case GLFW_RELEASE:
			return Input::Release;
		default:
			assert(false);
			Logger::warningf("Attempted to convert unhandled input action value");
			return Input::NumActions;
		}
	}

	int modifierFromGlfw(int mods)
	{
		int totalMods = 0;

		if (mods & GLFW_MOD_SHIFT)
		{
			totalMods |= Key::Shift;
		}

		if (mods & GLFW_MOD_CONTROL)
		{
			totalMods |= Key::Ctrl;
		}
			
		if (mods & GLFW_MOD_ALT)
		{
			totalMods |= Key::Alt;
		}
	
		return totalMods;
	}

	MouseButton mouseButtonFromGlfw(int button)
	{

#define IMPL_BT_SWITCH(glfw, value) \
		case glfw: \
		{ \
			return value; \
		} \

		switch (button)
		{
			IMPL_BT_SWITCH(GLFW_MOUSE_BUTTON_LEFT, MouseButton::Left)
			IMPL_BT_SWITCH(GLFW_MOUSE_BUTTON_RIGHT, MouseButton::Right)
			IMPL_BT_SWITCH(GLFW_MOUSE_BUTTON_MIDDLE, MouseButton::Middle)
			IMPL_BT_SWITCH(GLFW_MOUSE_BUTTON_4, MouseButton::Mouse4)
			IMPL_BT_SWITCH(GLFW_MOUSE_BUTTON_5, MouseButton::Mouse5)
		default:
			{
				Logger::warningf("Attempted to convert unhandled button value: %d", button);
				return MouseButton::NumButtons;
			}
		}
	}

	void updateMousePos(GlRenderWindow* renderWindow);

	namespace Platform
	{
		void pollEvents()
		{
			glfwPollEvents(); 
			
			for (GlRenderWindow* window : g_impl->m_pWindows)
			{
				updateMousePos(window);
			}
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

		Key::Value value = keyTypeFromGlfw(key, scancode);
		renderWindow->m_keyStates[value].m_action = actionFromGlfw(action);
		renderWindow->m_keyStates[value].m_modifiers = modifierFromGlfw(mods);
	}
	
	void updateMousePos(GlRenderWindow* renderWindow)
	{
		double x, y;
		glfwGetCursorPos(renderWindow->m_glfwWindow, &x, &y);

		renderWindow->m_mouseState.m_prev_x = renderWindow->m_mouseState.m_x;
		renderWindow->m_mouseState.m_prev_y = renderWindow->m_mouseState.m_y;
		renderWindow->m_mouseState.m_x = static_cast<float>(x);
		renderWindow->m_mouseState.m_y = static_cast<float>(y);
	}

	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		(void)mods; 
		GlRenderWindow* renderWindow = windowFromGlfw(window);
		MouseState& state = renderWindow->m_mouseState;
		state.m_buttonStates[mouseButtonFromGlfw(button)] = actionFromGlfw(action);
	}
}
