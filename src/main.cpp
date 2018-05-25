#include <assert.h>
#include <chrono>

#include "Tests/MathTests.hpp"
#include "Tests/MemoryTests.hpp"

#include "Core/Logger.hpp"
#include "Core/FileSystem.hpp"
#include "Core/Serialize.hpp"
#include "Core/Camera.hpp"
#include "Core/AssetRegistry.hpp"

#include "Core/Windows/PlatformWindows.hpp"

#include "Math/PhiMath.hpp"

#include "Render/RIOpenGL/RIOpenGL.hpp"
#include "Render/DeferredRenderer.hpp"
#include "Render/LightBuffer.hpp"

#include "Core/Components/CDirectionalLight.hpp"
#include "Core/Components/CStaticMesh.hpp"
#include "Core/Components/CTransform.hpp"
#include "Core/Components/CPointLight.hpp"

#include "UI/PhiImGui.h"

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

	EntityHandle createPointLightEntity(World* world, const Vec3& position, float radius, const Vec3& color, float intensity)
	{
		EntityHandle entity = world->createEntity();

		CTransform* tf = world->addComponent<CTransform>(entity);
		tf->m_translation = position;
		world->addComponent<CPointLight>(entity, color, radius, intensity);

		return entity;
	}

	void moveCamera(Camera* camera, const KbState& kbstate, float dt)
	{
		static Vec2 cameraVelocity;
		const float cameraAccel = 15.0f;
		const float cameraDrag = 0.85f;
		const float cameraVelMax = 50.0f;

		cameraVelocity *= cameraDrag;

		float cameraAccelThisFrame = cameraAccel;

		Input::Action state_a = kbstate[Key::A].m_action;
		if (state_a == Input::Press || state_a == Input::Repeat)
		{
			cameraVelocity.x -= cameraAccelThisFrame;
		}

		Input::Action state_d = kbstate[Key::D].m_action;
		if (state_d == Input::Press || state_d == Input::Repeat)
		{
			cameraVelocity.x += cameraAccelThisFrame;
		}

		Input::Action state_w = kbstate[Key::W].m_action;
		if (state_w == Input::Press || state_w == Input::Repeat)
		{
			cameraVelocity.y -= cameraAccelThisFrame;
		}

		Input::Action state_s = kbstate[Key::S].m_action;
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

		camera->moveRight(cameraVelocity.x * dt);
		camera->moveForward(cameraVelocity.y * dt);
	}

	void lookCamera(Camera* camera, const MouseState& mousestate, float dt)
	{
		if (mousestate.m_buttonStates[MouseButton::Left] == Input::Press)
		{
			float dx = mousestate.m_x - mousestate.m_prev_x;
			float dy = mousestate.m_y - mousestate.m_prev_y;

			dx = -radians(dx / 10.0f);
			dy = -radians(dy / 10.0f);

			camera->yaw(dx);
			camera->pitch(dy);
		}
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
	config.width = 1600;
	config.height = 900;
	config.title = "Phoenix";
	config.bFullscreen = false;

	RenderWindow* gameWindow = RI::createWindow(config);

	RI::setWindowToRenderTo(gameWindow);

	initImGui(gameWindow);

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
	world.registerComponentType<CPointLight>();

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

	createPointLightEntity(&world, Vec3(0.0, 2.0, -5.0), 1000.0f, Vec3(1.0, 1.0, 1.0), 1000.0f);

	using Clock = std::chrono::high_resolution_clock;
	using pointInTime = std::chrono::time_point<std::chrono::high_resolution_clock>;
	
	pointInTime lastTime = Clock::now();
	
	Platform::pollEvents();

	Camera camera;	
	LightBuffer lightBuffer;

	while (!gameWindow->wantsToClose())
	{
		Platform::pollEvents();
		
		UiInputConsume ioConsumed = filterUiConsumedInput();

		if (ioConsumed.m_bConsumedKeyboard)
		{
			resetKbState(&gameWindow->m_keyStates);
		}

		if (ioConsumed.m_bConsumedMouse)
		{
			resetMouseState(&gameWindow->m_mouseState);
		}
		
		startFrameImGui();

		pointInTime currentTime = Clock::now();
		std::chrono::duration<float> timeSpan = currentTime - lastTime;
		float dt = timeSpan.count();
		lastTime = currentTime;

		moveCamera(&camera, gameWindow->m_keyStates, dt);
		lookCamera(&camera, gameWindow->m_mouseState, dt);
		
		viewTf = camera.getUpdatedViewMatrix();
		renderer.setViewMatrix(viewTf);
		renderer.setupGBufferPass();

		for (CStaticMesh& mesh : ComponentIterator<CStaticMesh>(&world))
		{
			CTransform* transform = mesh.sibling<CTransform>();
			renderer.drawStaticMesh(mesh.m_mesh, transform->m_cached);
		}

		renderer.setupDirectLightingPass();
		lightBuffer.clear();
		
		for (CDirectionalLight& dirLight : ComponentIterator<CDirectionalLight>(&world))
		{
			lightBuffer.addDirectional(viewTf * dirLight.m_direction, dirLight.m_color);
		}

		for (CPointLight& pointLight : ComponentIterator<CPointLight>(&world))
		{
			CTransform* transform = pointLight.sibling<CTransform>();

			Vec4 eyePos(transform->m_translation, 1.0);
			eyePos *= viewTf;

			lightBuffer.addPointLight(Vec3(eyePos), pointLight.m_radius, pointLight.m_color, pointLight.m_intensity);
		}

		renderer.runLightsPass(lightBuffer);
		renderer.copyFinalColorToBackBuffer();

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		renderImGui();

		RI::swapBufferToWindow(gameWindow);
	}

	exitImGui();
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
