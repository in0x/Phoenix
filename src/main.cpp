#include <assert.h>
#include <chrono>

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

#include "Render/RIOpenGL/OpenGL.hpp"

#include "Core/Serialize.hpp"

#include "ThirdParty/dirent/dirent.h"

namespace Phoenix
{
	class ISystem
	{
	public:
		virtual ~ISystem() {}
		virtual void tick(World* world, float dt) = 0;
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

#define PHI_WRITE 0
#define PHI_LOAD 1

	// write
#if PHI_WRITE
	{
		std::vector<EntityHandle> sponza = meshEntitiesFromObj(&world, renderDevice, renderContext, "Models/sponza/sponza.obj");

		for (CStaticMesh& mesh : ComponentIterator<CStaticMesh>(&world))
		{
			WriteArchive ar;
			createWriteArchive(sizeof(StaticMesh), &ar);

			std::string savePath = "SerialTest/" + mesh.m_mesh.m_name + ".sm";

			serialize(ar, mesh.m_mesh);

			writeArchiveToDisk(savePath.c_str(), ar);
			destroyArchive(ar);
		}
	}
#endif // WRITE

#if PHI_LOAD
	// load
	{
		std::string sponzaPath("SerialTest/");

		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir(sponzaPath.c_str())) != NULL)
		{
			/* print all the files and directories within directory */
			while ((ent = readdir(dir)) != NULL)
			{
				const char* fileDot = strrchr(ent->d_name, '.');
				if (nullptr == fileDot)
				{
					continue;
				}

				if (strcmp(".sm", fileDot) != 0)
				{
					continue;
				}

				StaticMesh sm = loadStaticMesh((sponzaPath + ent->d_name).c_str(), renderDevice, renderContext);
				createMeshEntity(&world, std::move(sm), renderDevice, renderContext);
			}
			closedir(dir);
		}
		else
		{
			/* could not open directory */
			perror("");
			return EXIT_FAILURE;
		}
	}
#endif // LOAD

	EntityHandle light = world.createEntity();
	world.addComponent<CDirectionalLight>(light, Vec3(-0.5f, -0.5f, 0.f), Vec3(0.4f, 0.4f, 0.4f));

	EntityHandle light2 = world.createEntity();
	world.addComponent<CDirectionalLight>(light2, Vec3(0.5f, -0.5f, 0.f), Vec3(0.4f, 0.4f, 0.4f));

	using Clock = std::chrono::high_resolution_clock;
	using pointInTime = std::chrono::time_point<std::chrono::high_resolution_clock>;

	pointInTime lastTime = Clock::now();

	Platform::pollEvents();

	float prevMouseX = gameWindow->m_mouseState.m_x;
	float prevMouseY = gameWindow->m_mouseState.m_y;

	Camera camera;

	int numMesh = 0;


	while (!gameWindow->wantsToClose())
	{
		Platform::pollEvents();

		pointInTime currentTime = Clock::now();
		std::chrono::duration<float> timeSpan = currentTime - lastTime;
		float dt = timeSpan.count();

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


