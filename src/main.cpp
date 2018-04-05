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

//namespace Phoenix
//{
	/*
		Linear arrays of components
		Deletion by pop and swap -> allows for linear runs over arrays
		Entities -> ComponentIndex lookup map
		Components have type ids
		Register in world with type ids
		Type ids used to lookup component arrays
		Put the ID in an enum for now -> simple and centralized
	*/

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

	RenderMesh foxMesh = loadRenderMesh("Models/Fox/RedFox.obj", renderDevice);
	//RenderMesh foxMesh = loadRenderMesh("Models/sponza/sponza.obj", renderDevice);

	Material foxMaterial = Material(Vec3(1.f, 1.f, 1.f), Vec3(0.3f, 0.3f, 0.3f), 100.f);
	EntityHandle fox = world.createEntity();
	world.addComponent<CStaticMesh>(fox, foxMesh, foxMaterial);
	world.addComponent<CTransform>(fox);
	
	EntityHandle redLight = world.createEntity();
	world.addComponent<CDirectionalLight>(redLight, Vec3(0.f, -1.f, 0.f), Vec3(0.3f, 0.f, 0.f));
	
	EntityHandle blueLight = world.createEntity();
	world.addComponent<CDirectionalLight>(blueLight, Vec3(1.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f));

	EntityHandle greenLight = world.createEntity();
	world.addComponent<CDirectionalLight>(greenLight, Vec3(0.f, 1.f, 0.f), Vec3(0.f, 1.f, 0.f));

	DrawStaticMeshSystem drawMeshSystem(&renderer);
	DirectionalLightSystem dirLightSystem(&renderer);
	RotatorSystem rotator(0.5f);

	using Clock = std::chrono::high_resolution_clock;
	using pointInTime = std::chrono::time_point<std::chrono::high_resolution_clock>;
	 
	pointInTime lastTime = Clock::now();

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
				cameraUpdate.y += cameraChange * dt;
			}

			if (ev->m_value == Key::S && (ev->m_action == Key::Press || ev->m_action == Key::Repeat))
			{
				cameraUpdate.y -= cameraChange * dt;
			}
		}

		if (cameraUpdate.length2() > 0.0f)
		{
			cameraPos += cameraUpdate;

			Matrix4 viewTf = lookAtRH(cameraPos, cameraPos + Vec3(0.f, 0.f, -1.0f), Vec3(0.f, 1.f, 0.f));
			renderer.setViewMatrix(viewTf);
		}

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


