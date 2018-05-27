#include <assert.h>
#include <chrono>

#include "Tests/MathTests.hpp"
#include "Tests/MemoryTests.hpp"
#include "Render/RIOpenGL/RIOpenGL.hpp"

#include "Core/Texture.hpp"
#include "Core/Material.hpp"
#include "Core/Mesh.hpp"
#include "Core/ObjImport.hpp"
#include "Core/AssetRegistry.hpp"

#include "Core/Logger.hpp"
#include "Core/FileSystem.hpp"
#include "Core/Serialize.hpp"
#include "Core/Camera.hpp"
#include "Core/Mesh.hpp"
#include "Core/Windows/PlatformWindows.hpp"

#include "Math/PhiMath.hpp"

#include "Render/DeferredRenderer.hpp"
#include "Render/LightBuffer.hpp"

#include "UI/PhiImGui.h"

namespace Phoenix
{
	struct Transform 
	{
		Transform()
			: m_translation(0.f, 0.f, 0.f)
			, m_rotation(0.f, 0.f, 0.f)
			, m_scale(1.f, 1.f, 1.f)
		{
			recalculate();
		}

		Vec3 m_translation;
		Vec3 m_scale;
		Vec3 m_rotation;
		Matrix4 m_cached;

		const Matrix4& recalculate()
		{
			m_cached = Matrix4::translation(m_translation)
				* Matrix4::rotation(m_rotation)
				* Matrix4::scale(m_scale);

			return m_cached;
		}
	};

	struct StaticMeshEntity
	{
		StaticMesh* m_mesh;
		Transform m_transform;
	};

	struct DirectionalLight 
	{
		DirectionalLight(const Vec3& direction, const Vec3& color)
			: m_direction(direction)
			, m_color(color)
		{}

		Vec3 m_direction;
		Vec3 m_color;
	};

	struct PointLight 
	{
		Vec3 m_color;
		float m_radius;
		float m_intensity;
	};

	struct PointLightEntity
	{
		PointLight m_pointLight;
		Transform m_transform;
	};

	struct World
	{
		std::vector<StaticMeshEntity> m_staticMeshEntities;
		std::vector<DirectionalLight> m_dirLights;
		std::vector<PointLightEntity> m_pointLightEntities;
	};

	void createMeshEntity(World* world, StaticMesh* mesh)
	{
		world->m_staticMeshEntities.emplace_back();
		StaticMeshEntity& sme = world->m_staticMeshEntities.back();
		sme.m_mesh = mesh;

		sme.m_transform.m_scale = 0.1f;
		sme.m_transform.recalculate();
	}
	
	void meshEntitiesFromObj(World* world, const char* meshPath, IRIDevice* renderDevice, IRIContext* renderContext, AssetRegistry* assets)
	{
		std::vector<StaticMesh*> meshes = importObj(meshPath, renderDevice, renderContext, assets);

		for (StaticMesh* mesh : meshes)
		{
			createMeshEntity(world, mesh);
		}
	}

	void createPointLightEntity(World* world, const Vec3& position, float radius, const Vec3& color, float intensity)
	{
		world->m_pointLightEntities.emplace_back();
		PointLightEntity& pl = world->m_pointLightEntities.back();

		pl.m_pointLight.m_color = color;
		pl.m_pointLight.m_radius = radius;
		pl.m_pointLight.m_intensity = intensity;
		pl.m_transform.m_translation = position;
		pl.m_transform.recalculate();
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

	AssetRegistry assets;

	loadAssetRegistry(&assets, "phoenix.assets", renderDevice , renderContext);

	importTexture(g_defaultWhiteTexPath, nullptr, renderDevice, renderContext, &assets);
	importTexture(g_defaultBlackTexPath, nullptr, renderDevice, renderContext, &assets);

	addDefaultMaterial(&assets);

#define PHI_WRITE 1
#define PHI_LOAD 0

#if PHI_WRITE
	{
		meshEntitiesFromObj(&world, "Models/sponza/sponza.obj", renderDevice, renderContext, &assets);
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

		for (StaticMeshEntity& entity : world.m_staticMeshEntities)
		{
			renderer.drawStaticMesh(*entity.m_mesh, entity.m_transform.m_cached);
		}

		renderer.setupDirectLightingPass();
		lightBuffer.clear();
		
		for (DirectionalLight& dl : world.m_dirLights)
		{
			lightBuffer.addDirectional(viewTf * dl.m_direction, dl.m_color);
		}

		for (PointLightEntity& entity : world.m_pointLightEntities)
		{
			Vec4 eyePos(entity.m_transform.m_translation, 1.0);
			eyePos *= viewTf;

			lightBuffer.addPointLight(Vec3(eyePos), entity.m_pointLight.m_radius, entity.m_pointLight.m_color, entity.m_pointLight.m_intensity);
		}

		renderer.runLightsPass(lightBuffer);
		renderer.copyFinalColorToBackBuffer();

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		renderImGui();

		RI::swapBufferToWindow(gameWindow);
	}

	//saveAssetRegistry(assets, "phoenix.assets");

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
