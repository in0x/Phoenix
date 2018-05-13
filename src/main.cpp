#include <assert.h>
#include <chrono>

#include "Tests/MathTests.hpp"
#include "Tests/MemoryTests.hpp"

#include "Core/Logger.hpp"
#include "Core/FileSystem.hpp"
#include "Core/StringTokenizer.hpp"
#include "Core/Serialize.hpp"
#include "Core/Camera.hpp"
#include "Core/AssetRegistry.hpp"

#include "Core/Windows/PlatformWindows.hpp"

#include "Math/PhiMath.hpp"

#include "Render/RIOpenGL/RIOpenGL.hpp"
#include "Render/DeferredRenderer.hpp"

#include "Core/Components/CDirectionalLight.hpp"
#include "Core/Components/CStaticMesh.hpp"
#include "Core/Components/CTransform.hpp"

namespace Phoenix
{
	std::vector<EntityHandle> meshEntitiesFromObj(World* world, IRIDevice* renderDevice, const char* meshPath, AssetRegistry* assets)
	{
		std::vector<EntityHandle> entities;
		std::vector<StaticMesh> meshes = importObj(meshPath, renderDevice, assets);

		for (StaticMesh& mesh : meshes)
		{
			EntityHandle entity = world->createEntity();
			world->addComponent<CStaticMesh>(entity, mesh);
			world->addComponent<CTransform>(entity);
			entities.push_back(entity);
		}

		return entities;
	}

	EntityHandle createMeshEntity(World* world, StaticMesh&& mesh)
	{
		EntityHandle entity = world->createEntity();
		world->addComponent<CStaticMesh>(entity, std::move(mesh));
		CTransform* tf = world->addComponent<CTransform>(entity);
		tf->m_scale = 0.1f;
		tf->recalculate();
		return entity;
	}
}

void run()
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
	Vec3 cameraUp = Vec3(0.f, 1.f, 0.f);
	Matrix4 viewTf = lookAtRH(cameraPos, (cameraPos + cameraForward).normalized(), cameraUp);
	renderer.setViewMatrix(viewTf);

	Matrix4 projTf = perspectiveRH(70.f, (float)config.width / (float)config.height, 0.1f, 10000.f);
	renderer.setProjectionMatrix(projTf);

	World world;
	world.registerComponentType<CDirectionalLight>();
	world.registerComponentType<CStaticMesh>();
	world.registerComponentType<CTransform>();

	AssetRegistry assets(renderDevice, renderContext);

#define PHI_WRITE 0
#define PHI_LOAD 1

#if PHI_WRITE
	{
		std::vector<EntityHandle> sponza = meshEntitiesFromObj(&world, renderDevice, "Models/sponza/sponza.obj", &assets);

		for (CStaticMesh& mesh : ComponentIterator<CStaticMesh>(&world))
		{
			std::string savePath = "SerialTest/" + mesh.m_mesh.m_name + ".sm";
			saveStaticMesh(mesh.m_mesh, savePath.c_str());
		}
	}
#endif // PHI_WRITE

#if PHI_LOAD 
	const char* sponzaPath = "SerialTest/";

	std::vector<std::string> sponzaFiles = getFilesInDirectory(sponzaPath);

	for (const std::string& file : sponzaFiles)
	{
		if (!hasExtension(file.c_str(), ".sm"))
		{
			continue;
		}

		StaticMesh sm = loadStaticMesh((sponzaPath + file).c_str(), renderDevice, &assets);
		createMeshEntity(&world, std::move(sm));
	}
#endif // PHI_LOAD

	EntityHandle light = world.createEntity();
	world.addComponent<CDirectionalLight>(light, Vec3(-0.5f, -0.5f, -0.5f), Vec3(253.0 / 255.0, 230.0 / 255.0, 155.0 / 255.0) * 5.0);
	//world.addComponent<CDirectionalLight>(light, Vec3(0.0f, -1.0f, 0.0f), Vec3(5.0f, 5.0f, 5.0f));

	using Clock = std::chrono::high_resolution_clock;
	using pointInTime = std::chrono::time_point<std::chrono::high_resolution_clock>;
	
	pointInTime lastTime = Clock::now();
	
	Platform::pollEvents();

	float prevMouseX = gameWindow->m_mouseState.m_x;
	float prevMouseY = gameWindow->m_mouseState.m_y;

	Camera camera;

	Vec2 cameraVelocity;
	float cameraAccel = 15.0f;
	float cameraDrag = 0.85f;
	float cameraVelMax = 50.0f;

	while (!gameWindow->wantsToClose())
	{
		Platform::pollEvents();

		pointInTime currentTime = Clock::now();
		std::chrono::duration<float> timeSpan = currentTime - lastTime;
		float dt = timeSpan.count();
		lastTime = currentTime;

		cameraVelocity *= cameraDrag;

		float cameraAccelThisFrame = cameraAccel;

		Input::Action state_a = gameWindow->m_keyStates[Key::A].m_action;
		if (state_a == Input::Press || state_a == Input::Repeat)
		{
			cameraVelocity.x -= cameraAccelThisFrame;
		}

		Input::Action state_d = gameWindow->m_keyStates[Key::D].m_action;
		if (state_d == Input::Press || state_d == Input::Repeat)
		{
			cameraVelocity.x += cameraAccelThisFrame;
		}

		Input::Action state_w = gameWindow->m_keyStates[Key::W].m_action;
		if (state_w == Input::Press || state_w == Input::Repeat)
		{
			cameraVelocity.y -= cameraAccelThisFrame;
		}

		Input::Action state_s = gameWindow->m_keyStates[Key::S].m_action;
		if (state_s == Input::Press || state_s == Input::Repeat)
		{
			cameraVelocity.y += cameraAccelThisFrame;
		}

		float cameraVelLen = cameraVelocity.length();

		if (cameraVelLen > cameraVelMax)
		{
			cameraVelocity.normalize();
			cameraVelocity *= cameraVelMax;
		}

		camera.moveRight(cameraVelocity.x * dt);
		camera.moveForward(cameraVelocity.y * dt);

		MouseState mouse = gameWindow->m_mouseState;

		if (mouse.m_buttonStates[MouseState::Left] == Input::Press)
		{
			float dx = mouse.m_x - prevMouseX;
			float dy = mouse.m_y - prevMouseY;

			dx = -radians(dx / 10.0f);
			dy = -radians(dy / 10.0f);

			camera.yaw(dx);
			camera.pitch(dy);
		}

		prevMouseX = mouse.m_x;
		prevMouseY = mouse.m_y;

		renderer.setViewMatrix(camera.updateViewMatrix());

		renderer.setupGBufferPass();

		for (CStaticMesh& mesh : ComponentIterator<CStaticMesh>(&world))
		{
			CTransform* transform = mesh.sibling<CTransform>();

			renderer.drawStaticMesh(mesh.m_mesh, transform->m_cached);
		}

		renderer.setupAmbientLightPass();
		renderer.drawAmbientLight();

		renderer.setupDirectionalLightPass();

		for (CDirectionalLight& dirLight : ComponentIterator<CDirectionalLight>(&world))
		{
			renderer.drawDirectionalLight(dirLight.m_direction, dirLight.m_color);
		}

		renderer.copyFinalColorToBackBuffer();

		RI::swapBufferToWindow(gameWindow);
	}

	RI::destroyWindow(gameWindow);
	RI::exit();
	Logger::exit();

}

int main(int argc, char** argv)
{
#define UNUSED(x) (void)x
	UNUSED(argc);
	UNUSED(argv);

	run();

	return 0;
}
