#include <assert.h>

#include "Tests/MathTests.hpp"
#include "Tests/MemoryTests.hpp"

#include "Core/Logger.hpp"
#include "Core/FileSystem.hpp"
#include "Core/StringTokenizer.hpp"
#include "Core/Serialize.hpp"
#include "Core/Clock.hpp"
#include "Core/Camera.hpp"

#include "Core/Windows/PlatformWindows.hpp"

#include "Math/PhiMath.hpp"

#include "Render/RIOpenGL/RIOpenGL.hpp"
#include "Render/DeferredRenderer.hpp"

#include "Core/Components/CDirectionalLight.hpp"
#include "Core/Components/CStaticMesh.hpp"
#include "Core/Components/CTransform.hpp"


namespace Phoenix
{
	std::vector<EntityHandle> meshEntitiesFromObj(World* world, IRIDevice* renderDevice, IRIContext* renderContext, const char* meshPath)
	{
		std::vector<EntityHandle> entities;
		std::vector<StaticMesh> meshes = importObj(meshPath, renderDevice, renderContext);

		for (StaticMesh& mesh : meshes)
		{
			EntityHandle entity = world->createEntity();
			world->addComponent<CStaticMesh>(entity, mesh);
			world->addComponent<CTransform>(entity);
			entities.push_back(entity);
		}

		return entities;
	}

	EntityHandle createMeshEntity(World* world, StaticMesh&& mesh, IRIDevice* renderDevice, IRIContext* renderContext)
	{
		EntityHandle entity = world->createEntity();
		world->addComponent<CStaticMesh>(entity, std::move(mesh));
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
	Tests::runSerializeTests();

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
	config.width = 1280;
	config.height = 720;
	config.title = "Phoenix";
	config.bFullscreen = false;

	RenderWindow* gameWindow = RI::createWindow(config);

	RI::setWindowToRenderTo(gameWindow);

	DeferredRenderer renderer(renderDevice, renderContext, config.width, config.height);

	Vec3 cameraPos(0.f, 0.f, 7.f);
	Vec3 cameraForward(0.0f, 0.0f, 1.0f);
	Matrix4 viewTf = lookAtRH(cameraPos, (cameraPos + cameraForward).normalized(), Vec3(0.f, 1.f, 0.f));
	renderer.setViewMatrix(viewTf);

	Matrix4 projTf = perspectiveRH(70.f, (float)config.width / (float)config.height, 0.1f, 10000.f);
	renderer.setProjectionMatrix(projTf);

	World world;
	world.registerComponentType<CDirectionalLight>();
	world.registerComponentType<CStaticMesh>();
	world.registerComponentType<CTransform>();

	const char* sponzaPath = "SerialTest/";

	std::vector<std::string> sponzaFiles = getFilesInDirectory(sponzaPath);

	for (const std::string& file : sponzaFiles)
	{
		if (!hasExtension(file.c_str(), ".sm"))
		{
			continue;
		}

		StaticMesh sm = loadStaticMesh((sponzaPath + file).c_str(), renderDevice, renderContext);
		createMeshEntity(&world, std::move(sm), renderDevice, renderContext);
	}	

	EntityHandle light = world.createEntity();
	world.addComponent<CDirectionalLight>(light, Vec3(-0.5f, -0.5f, 0.f), Vec3(0.4f, 0.4f, 0.4f));

	EntityHandle light2 = world.createEntity();
	world.addComponent<CDirectionalLight>(light2, Vec3(0.5f, -0.5f, 0.f), Vec3(0.4f, 0.4f, 0.4f));

	Clock clock;
	clock.start();
	
	double lastTime = clock.getElapsedS();

	Platform::pollEvents();

	float prevMouseX = gameWindow->m_mouseState.m_x;
	float prevMouseY = gameWindow->m_mouseState.m_y;

	Camera camera;

	while (!gameWindow->wantsToClose())
	{
		Platform::pollEvents();

		double currentTime = clock.getElapsedS();
		double dt = currentTime - lastTime;
		
		float cameraChange = 0.5f * dt;

		while (!gameWindow->m_keyEvents.isEmpty())
		{
			Key::Event* ev = gameWindow->m_keyEvents.get();

			if (ev->m_value == Key::A && (ev->m_action == Input::Press || ev->m_action == Input::Repeat))
			{
				camera.moveRight(-cameraChange);
			}

			if (ev->m_value == Key::D && (ev->m_action == Input::Press || ev->m_action == Input::Repeat))
			{
				camera.moveRight(cameraChange);
			}

			if (ev->m_value == Key::W && (ev->m_action == Input::Press || ev->m_action == Input::Repeat))
			{
				camera.moveForward(-cameraChange);
			}

			if (ev->m_value == Key::S && (ev->m_action == Input::Press || ev->m_action == Input::Repeat))
			{
				camera.moveForward(cameraChange);
			}
		}

		MouseState mouse = gameWindow->m_mouseState;

		if (mouse.m_x != prevMouseX || mouse.m_y != prevMouseY)
		{
			if (mouse.m_buttonStates[MouseState::Left] == Input::Press)
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

		renderer.setupGBufferPass();

		for (CStaticMesh& mesh : ComponentIterator<CStaticMesh>(&world))
		{
			CTransform* transform = mesh.sibling<CTransform>();

			renderer.drawStaticMesh(mesh.m_mesh, transform->m_cached);
		}

		renderer.setupLightPass();

		for (CDirectionalLight& dirLight : ComponentIterator<CDirectionalLight>(&world))
		{
			renderer.drawLight(dirLight.m_direction, dirLight.m_color);
		}

		renderer.copyFinalColorToBackBuffer();

		RI::swapBufferToWindow(gameWindow);
	}

	RI::destroyWindow(gameWindow);
	RI::exit();
	Logger::exit();

	return 0;
}


