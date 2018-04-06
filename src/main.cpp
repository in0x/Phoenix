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
	
	Vec3 cameraPos = Vec3(0.f, 0.f, 7.f);
	Matrix4 viewTf = lookAtRH(cameraPos, Vec3(0.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
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
	
	DrawStaticMeshSystem drawMeshSystem(&renderer);
	DirectionalLightSystem dirLightSystem(&renderer);
	RotatorSystem rotator(0.5f);

	using Clock = std::chrono::high_resolution_clock;
	using pointInTime = std::chrono::time_point<std::chrono::high_resolution_clock>;
	 
	pointInTime lastTime = Clock::now();

	float prevMouseX = 0.0f;
	float prevMouseY = 0.0f;

	while (!window->wantsToClose())
	{
		Platform::pollEvents();

		pointInTime currentTime = Clock::now();
		std::chrono::duration<float> timeSpan = currentTime - lastTime;
		float dt = timeSpan.count();

		Vec3 cameraUpdate;
		float cameraChange = 0.05f;

		while (!window->m_keyEvents.isEmpty())
		{
			Key::Event* ev = window->m_keyEvents.get();

			if (ev->m_value == Key::A && (ev->m_action == Key::Press || ev->m_action == Key::Repeat))
			{
				cameraUpdate.x -= cameraChange * dt;
			}

			if (ev->m_value == Key::D && (ev->m_action == Key::Press || ev->m_action == Key::Repeat))
			{
				cameraUpdate.x += cameraChange * dt;
			}

			if (ev->m_value == Key::W && (ev->m_action == Key::Press || ev->m_action == Key::Repeat))
			{
				cameraUpdate.z += cameraChange * dt;
			}

			if (ev->m_value == Key::S && (ev->m_action == Key::Press || ev->m_action == Key::Repeat))
			{
				cameraUpdate.z -= cameraChange * dt;
			}
		}

		if (cameraUpdate.length2() > 0.0f)
		{
			cameraPos += cameraUpdate;

			Matrix4 viewTf = lookAtRH(cameraPos, cameraPos + Vec3(0.f, 0.f, -1.0f), Vec3(0.f, 1.f, 0.f));
			renderer.setViewMatrix(viewTf);
		}

		MouseState mouse = window->m_mouseState;

		if (mouse.m_x != prevMouseX || mouse.m_y != prevMouseY)
		{
			prevMouseX = mouse.m_x;
			prevMouseY = mouse.m_y;

			Logger::logf("New mouse pos: X -> %f	Y -> %f", mouse.m_x, mouse.m_y);
		}

		rotator.tick(&world, 0);
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


