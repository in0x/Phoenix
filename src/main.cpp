#include <assert.h>

#include "Tests/MathTests.hpp"
#include "Tests/MemoryTests.hpp"

#include "Core/Logger.hpp"
#include "Core/Windows/PlatformWindows.hpp"

#include "Math/PhiMath.hpp"

#include "Render/RIOpenGL/RIOpenGL.hpp"
#include "Render/DeferredRenderer.hpp"

#include "Core/Components/CDirectionalLight.hpp"
#include "Core/Components/CStaticMesh.hpp"
#include "Core/Components/CTransform.hpp"

#include <chrono>

template <typename T>
T clamp(T lo, T v, T hi)
{
	return v < lo ? lo : hi < v ? hi : v;
}

namespace Phoenix
{
	class ISystem
	{
	public:
		virtual ~ISystem() {}
		virtual void tick(World* world, float dt) = 0;
	};

	class DrawStaticMeshSystem : public ISystem
	{
	public:
		DrawStaticMeshSystem(DeferredRenderer* renderer)
			: m_renderer(renderer)
		{}

		virtual void tick(World* world, float dt) override
		{
			m_renderer->setupGBufferPass();

			for (CStaticMesh& mesh : ComponentIterator<CStaticMesh>(world))
			{
				CTransform* transform = mesh.sibling<CTransform>();

				m_renderer->drawStaticMesh(mesh.m_mesh, transform->toMat4(), mesh.m_material);
			}
		}

	private:
		DeferredRenderer* m_renderer;
	};

	class DirectionalLightSystem : public ISystem
	{
	public:
		DirectionalLightSystem(DeferredRenderer* renderer)
			: m_renderer(renderer)
		{}

		virtual void tick(World* world, float dt) override
		{
			m_renderer->setupLightPass();

			for (CDirectionalLight& dirLight : ComponentIterator<CDirectionalLight>(world))
			{
				m_renderer->drawLight(dirLight.m_direction, dirLight.m_color);
			}
		}

	private:
		DeferredRenderer* m_renderer;
	};

	class RotatorSystem : public ISystem
	{
	public:
		RotatorSystem(float speed)
			: m_speed(speed)
		{}

		virtual void tick(World* world, float dt) override
		{
			for (CTransform& transform : ComponentIterator<CTransform>(world))
			{
				transform.m_rotation.y += m_speed;
			}
		}
		float m_speed;
	};

	EntityHandle createMeshEntity(World* world, IRIDevice* renderDevice, const char* meshPath)
	{
		RenderMesh mesh = loadRenderMesh(meshPath, renderDevice);
		Material material = Material(Vec3(1.f, 1.f, 1.f), Vec3(0.3f, 0.3f, 0.3f), 100.f);

		EntityHandle entity = world->createEntity();
		world->addComponent<CStaticMesh>(entity, mesh, material);
		world->addComponent<CTransform>(entity);

		return entity;
	}

	struct Camera
	{
		Vec3 m_position;
		Vec3 m_xAxis;
		Vec3 m_zAxis; 
		Vec3 m_yAxis;

		Camera()
			: m_position(0.f, 0.f, 0.f)
			, m_xAxis(1.0f, 0.0f, 0.0f)
			, m_zAxis(0.0f, 0.0f, 1.0f)
			, m_yAxis(0.0f, 1.0f, 0.0f)
		{}

		void moveRight(float d) 
		{
			m_position += m_xAxis * d;
		}

		void moveForward(float d) 
		{
			m_position += m_zAxis * d;
		}

		void pitch(float angle)
		{
			Matrix4 r = Matrix4::rotation(angle, m_xAxis);
			m_yAxis = r * m_yAxis;
			m_zAxis = r * m_zAxis;
		}

		void yaw(float angle)
		{
			Matrix4 r = Matrix4::rotation(angle, Vec3(0.0f, 1.0f, 0.0f));
			m_xAxis = r * m_xAxis;
			m_zAxis = r * m_zAxis;
		}

		Matrix4 updateViewMatrix()
		{	
			m_zAxis = m_zAxis.normalize(); 
			m_yAxis = m_zAxis.cross(m_xAxis).normalized();
			m_xAxis = m_yAxis.cross(m_zAxis).normalized(); 

			return Matrix4{
				m_xAxis.x, m_xAxis.y, m_xAxis.z, -(m_xAxis.dot(m_position)),
				m_yAxis.x, m_yAxis.y, m_yAxis.z, -(m_yAxis.dot(m_position)),
				m_zAxis.x, m_zAxis.y, m_zAxis.z, -(m_zAxis.dot(m_position)),
				0,		 0,		  0,		1
			};
		}
	};
}


int main(int argc, char** argv)
{
	using namespace Phoenix;

	Logger::init(true, false, 1024);
	Logger::setAnsiColorEnabled(Platform::enableConsoleColor(true));

	Tests::runMathTests();
	Tests::runMemoryTests();

	bool bRIstarted = RI::init();

	if (!bRIstarted)
	{
		Logger::error("Failed to initialize RenderInterface");
		assert(false);
	}

	IRIDevice* renderDevice = RI::getRenderDevice();
	IRIContext* renderContext = RI::getRenderContex();
	renderContext->setDepthTest(EDepth::Enable);

	WindowConfig config;
	config.width = 800;
	config.height = 600;
	config.title = "Phoenix";
	config.bFullscreen = false;

	RenderWindow* window = RI::createWindow(config);

	RI::setWindowToRenderTo(window);

	DeferredRenderer renderer(renderDevice, renderContext, config.width, config.height);
	
	Vec3 cameraPos(0.f, 0.f, 7.f);
	Vec3 cameraForward(0.0f, 0.0f, 1.0f);
	Matrix4 viewTf = lookAtRH(cameraPos, (cameraPos + cameraForward).normalized(), Vec3(0.f, 1.f, 0.f));
	renderer.setViewMatrix(viewTf);

	Matrix4 projTf = perspectiveRH(70.f, (float)config.width / (float)config.height, 0.1f, 100.f);
	renderer.setProjectionMatrix(projTf);
	
	World world;
	world.registerComponentType<CDirectionalLight>();
	world.registerComponentType<CStaticMesh>();
	world.registerComponentType<CTransform>();

	EntityHandle fox = createMeshEntity(&world, renderDevice, "Models/Fox/RedFox.obj");
	
	EntityHandle light = world.createEntity();
	world.addComponent<CDirectionalLight>(light, Vec3(-0.5f, -0.5f, 0.f), Vec3(0.4f, 0.4f, 0.4f));
	
	EntityHandle light2 = world.createEntity();
	world.addComponent<CDirectionalLight>(light2, Vec3(0.5f, -0.5f, 0.f), Vec3(0.4f, 0.4f, 0.4f));

	DrawStaticMeshSystem drawMeshSystem(&renderer);
	DirectionalLightSystem dirLightSystem(&renderer);
	RotatorSystem rotator(0.5f);

	using Clock = std::chrono::high_resolution_clock;
	using pointInTime = std::chrono::time_point<std::chrono::high_resolution_clock>;
	 
	pointInTime lastTime = Clock::now();

	Platform::pollEvents();

	float prevMouseX = window->m_mouseState.m_x;
	float prevMouseY = window->m_mouseState.m_y;

	Camera camera;

	while (!window->wantsToClose())
	{
		Platform::pollEvents();

		pointInTime currentTime = Clock::now();
		std::chrono::duration<float> timeSpan = currentTime - lastTime;
		float dt = timeSpan.count();

		float cameraChange = 0.05f * dt;

		while (!window->m_keyEvents.isEmpty())
		{
			Key::Event* ev = window->m_keyEvents.get();

			if (ev->m_value == Key::A && (ev->m_action == Key::Press || ev->m_action == Key::Repeat))
			{
				camera.moveRight(-cameraChange);
			}

			if (ev->m_value == Key::D && (ev->m_action == Key::Press || ev->m_action == Key::Repeat))
			{
				camera.moveRight(cameraChange);
			}

			if (ev->m_value == Key::W && (ev->m_action == Key::Press || ev->m_action == Key::Repeat))
			{
				camera.moveForward(-cameraChange);
			}

			if (ev->m_value == Key::S && (ev->m_action == Key::Press || ev->m_action == Key::Repeat))
			{
				camera.moveForward(cameraChange);
			}
		}

		MouseState mouse = window->m_mouseState;

		if (mouse.m_x != prevMouseX || mouse.m_y != prevMouseY)
		{
			if (mouse.m_leftDown)
			{
				float sens = 25.0f;
				float dx = -radians(sens * ((mouse.m_x - prevMouseX) / config.width) * dt);
				float dy = -radians(sens * ((mouse.m_y - prevMouseY) / config.height) * dt);

				camera.yaw(dx);
				camera.pitch(dy);
			}

			prevMouseX = mouse.m_x;
			prevMouseY = mouse.m_y;
		}

		renderer.setViewMatrix(camera.updateViewMatrix());

		//rotator.tick(&world, 0);
		drawMeshSystem.tick(&world, 0);
		dirLightSystem.tick(&world, 0);

		renderContext->endPass();

		RI::swapBufferToWindow(window);
	}

	RI::destroyWindow(window);
	RI::exit();
	Logger::exit();

	return 0;
}


