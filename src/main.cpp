#include <assert.h>
#include <chrono>

#include "Tests/MathTests.hpp"
#include "Tests/MemoryTests.hpp"
#include "Render/RIOpenGL/RIOpenGL.hpp"

#include "Core/ObjImport.hpp"
#include "Core/AssetRegistry.hpp"

#include "Core/Logger.hpp"
#include "Core/Serialize.hpp"
#include "Core/SerialUtil.hpp"
#include "Core/Camera.hpp"
#include "Core/Windows/PlatformWindows.hpp"

#include "Core/Texture.hpp"
#include "Core/Material.hpp"
#include "Core/Mesh.hpp"
#include "Math/PhiMath.hpp"

#include "Memory/ChunkArray.hpp"

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
		Vec3 m_direction;
		Vec3 m_color;
	};

	struct DirLightEntity
	{
		DirectionalLight m_dirLight;
		Transform m_transform;
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
		std::vector<DirLightEntity>	  m_dirLightEntities;
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

	struct LoadResources
	{
		IRIDevice* device;
		IRIContext* context;
		AssetRegistry* assets;
	};

	void addPointLightToBuffer(const PointLight& pl, const Vec3& pos, LightBuffer* buffer)
	{
		buffer->addPointLight(pos, pl.m_radius, pl.m_color, pl.m_intensity);
	}

	void serialize(Archive& ar, float& f)
	{
		ar.serialize(&f, sizeof(float));
	}

	void serialize(Archive& ar, Vec3& vec3)
	{
		ar.serialize(&vec3, sizeof(vec3));
	}

	void serialize(Archive& ar, Vec4& vec4)
	{
		ar.serialize(&vec4, sizeof(vec4));
	}

	void serialize(Archive& ar, Transform& transform)
	{
		serialize(ar, transform.m_translation);
		serialize(ar, transform.m_scale);
		serialize(ar, transform.m_rotation);
	}

	void saveStaticMeshEntity(Archive& ar, StaticMeshEntity& entity)
	{
		serialize(ar, entity.m_transform);
		serialize(ar, entity.m_mesh->m_name);
	}

	void loadStaticMeshEntity(Archive& ar, StaticMeshEntity& entity, LoadResources* resources)
	{
		serialize(ar, entity.m_transform);

		std::string meshName;
		serialize(ar, meshName);

		entity.m_mesh = loadStaticMesh(meshName.c_str(), resources->device, resources->context, resources->assets);
		entity.m_transform.recalculate();
	}

	void serialize(Archive& ar, DirectionalLight& dl)
	{
		serialize(ar, dl.m_color);
		serialize(ar, dl.m_direction);
	}

	void saveDirLightEntity(Archive& ar, DirLightEntity& entity)
	{
		serialize(ar, entity.m_dirLight);
		serialize(ar, entity.m_transform);
	}

	void loadDirLightEntity(Archive& ar, DirLightEntity& entity)
	{
		serialize(ar, entity.m_dirLight);
		serialize(ar, entity.m_transform);

		entity.m_transform.recalculate();
	}

	void serialize(Archive& ar, PointLight& dl)
	{
		serialize(ar, dl.m_color);
		serialize(ar, dl.m_intensity);
		serialize(ar, dl.m_radius);
	}

	void savePointLightEntity(Archive& ar, PointLightEntity& entity)
	{
		serialize(ar, entity.m_pointLight);
		serialize(ar, entity.m_transform);
	}

	void loadPointLightEntity(Archive& ar, PointLightEntity& entity)
	{
		serialize(ar, entity.m_pointLight);
		serialize(ar, entity.m_transform);

		entity.m_transform.recalculate();
	}

	void saveWorld(World& world, const char* path)
	{
		WriteArchive ar;
		createWriteArchive(sizeof(StaticMesh), &ar);

		{
			size_t numStaticMeshEnts = world.m_staticMeshEntities.size();
			serialize(ar, numStaticMeshEnts);

			for (StaticMeshEntity& e : world.m_staticMeshEntities)
			{
				saveStaticMeshEntity(ar, e);
			}
		}

		{
			size_t numDirLightEnts = world.m_dirLightEntities.size();
			serialize(ar, numDirLightEnts);

			for (DirLightEntity& e : world.m_dirLightEntities)
			{
				saveDirLightEntity(ar, e);
			}
		}

		{
			size_t numPlEnts = world.m_pointLightEntities.size();
			serialize(ar, numPlEnts);

			for (PointLightEntity& e : world.m_pointLightEntities)
			{
				savePointLightEntity(ar, e);
			}
		}

		EArchiveError err = writeArchiveToDisk(path, ar);
		assert(err == EArchiveError::NoError);
		destroyArchive(ar);
	}

	void loadWorld(const char* path, World* outWorld, LoadResources* resources)
	{
		ReadArchive ar;
		EArchiveError err = createReadArchive(path, &ar);

		assert(err == EArchiveError::NoError);
		if (err != EArchiveError::NoError)
		{
			return;
		}

		{
			size_t numStaticMeshEnts = 0;
			serialize(ar, numStaticMeshEnts);

			for (size_t i = 0; i < numStaticMeshEnts; ++i)
			{
				outWorld->m_staticMeshEntities.emplace_back();
				loadStaticMeshEntity(ar, outWorld->m_staticMeshEntities.back(), resources);
			}
		}

		{
			size_t numDirLightEnts = 0;
			serialize(ar, numDirLightEnts);

			for (size_t i = 0; i < numDirLightEnts; ++i)
			{
				outWorld->m_dirLightEntities.emplace_back();
				loadDirLightEntity(ar, outWorld->m_dirLightEntities.back());
			}
		}

		{
			size_t numPlEnts = 0;
			serialize(ar, numPlEnts);

			for (size_t i = 0; i < numPlEnts; ++i)
			{
				outWorld->m_pointLightEntities.emplace_back();
				loadPointLightEntity(ar, outWorld->m_pointLightEntities.back());
			}
		}

		destroyArchive(ar);
	}
}

namespace Phoenix
{
	/*
	Define EntityType by set of components
	Entity Types are enum
	EntityType can be represented as enum bitmask (64 is fine for now)
	All entities of one EntityType are stored in contiguous storage (like The Machinery)
	Systems define a set of components they are interested in
	When updating systems, we run over all types and give system the entity
	containers that match the components it is interested in
	*/

	struct EComponent
	{
		static const uint64_t Transform = 1 << 0;
		static const uint64_t Health = 1 << 1;
	};

	struct TestTransformComponent
	{
		static const uint64_t s_type = EComponent::Transform;

		float x;
		float y;
		float z;
	};

	struct TestHealthComponent
	{
		static const uint64_t s_type = EComponent::Health;

		float health;
	};

	struct NewEntity
	{
		virtual uint64_t getComponentMask() const = 0;
		virtual void* getComponent(uint64_t type) = 0;

		template <typename T>
		T* getComponentT()
		{
			return reinterpret_cast<T*>(getComponent(T::s_type)); // Problem with nullptr?
		}
	};

	struct GeneratedEntity : public NewEntity
	{
		TestTransformComponent m_transform;
		TestHealthComponent m_health;

		virtual uint64_t getComponentMask() const override
		{
			return EComponent::Transform | EComponent::Health;
		}

		virtual void* getComponent(uint64_t type) override
		{
			// Could maybe be faster if we could figure out a way to add a lookup table indexed with type
			switch (type)
			{
			case EComponent::Transform:
			{
				return &m_transform;
			}

			case EComponent::Health:
			{
				return &m_health;
			}

			default:
			{
				return nullptr;
			}
			}
		}
	};

	// Generates ^^^^
	//IMPL_ENTITY(TransformComponent, m_transform, HealthComponent, m_health) // Do we want/need a typename?

#define IMPL_ENTITY_ONE_COMPONENT(TYPE, NAME) \
	struct TYPE ## Entity : public NewEntity \
	{ \
		TYPE NAME; \
		\
		virtual uint64_t getComponentMask() const override \
		{ \
			return TYPE ## ::s_type; \
		} \
		\
		virtual void* getComponent(uint64_t type) override \
		{ \
			switch (type) \
			{ \
				case TYPE ## ::s_type: \
				{ \
					return &NAME; \
				} \
				default: \
				{ \
					return nullptr; \
				} \
			} \
		} \
	}; 

#define IMPL_COMP_SWITCH(TYPE, NAME) \
	case TYPE ## ::s_type: \
	{ \
		return &NAME; \
	} \

#define IMPL_ENTITY_TWO_COMPONENTS(TYPE_ONE, NAME_ONE, TYPE_TWO, NAME_TWO) \
	struct TYPE_ONE ## TYPE_TWO ## Entity : public NewEntity \
	{ \
		TYPE_ONE NAME_ONE; \
		TYPE_TWO NAME_TWO; \
		\
		virtual uint64_t getComponentMask() const override \
		{ \
			return TYPE_ONE ## ::s_type | TYPE_TWO ## ::s_type; \
		} \
		\
		virtual void* getComponent(uint64_t type) override \
		{ \
			switch (type) \
			{ \
				IMPL_COMP_SWITCH(TYPE_ONE, NAME_ONE) \
				IMPL_COMP_SWITCH(TYPE_TWO, NAME_TWO) \
				default: \
				{ \
					return nullptr; \
				} \
			} \
		} \
	};

	IMPL_ENTITY_ONE_COMPONENT(TestTransformComponent, m_transform);
	IMPL_ENTITY_ONE_COMPONENT(TestHealthComponent, m_health);

	IMPL_ENTITY_TWO_COMPONENTS(TestTransformComponent, m_transform, TestHealthComponent, m_health);

#define PHI_COMPONENT_1_MEMBER(MEMBER_TYPE, MEMBER_NAME) TYPE ## NAME ; 

#define PHI_COMPONENT_2_MEMBER(MEMBER_TYPE_1, MEMBER_NAME_1, MEMBER_TYPE_2, MEMBER_NAME_2) \
	S2(PHI_COMPONENT_1_MEMBER(MEMBER_TYPE_1, MEMBER_NAME_1)) \ 
	S2(PHI_COMPONENT_1_MEMBER(MEMBER_TYPE_2, MEMBER_NAME_2)) \


	// Could feasibly generate all possible permutations (without order differences)

	// Easiset way that I know how to do this is like Unreal does its Delegates
	// Should split repeating snippets in these macros out into resuable expanded macros


	/*
	Big issue: Add/RemoveComponent requires being able to safely
	copy component data between Entity types. But memory order of components
	is not always going to be the same.

	Example

	Entity: [Transform, Mesh, Health]

	| RemoveComponent(EComponent::Mesh)

	Entity: [Transform, Health]

	Cant memcpy because there is a gap between Transform and Health in src data
	*/

	void copyEntity()
	{
		// Could probably copy by iterarting all component types,
		// checking if both entities have it, 
		// if yes, do e2->GetComponent<C>() = e1->GetComponent<C>()
		// Only problem is how do I iterate types
	}

	namespace Tests
	{
		void runEcsTests()
		{
			TestTransformComponentTestHealthComponentEntity entity;

			TestTransformComponent* tf = entity.getComponentT<TestTransformComponent>();

			return;
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
	Tests::runEcsTests();

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

	const char* worldPath = "Assets/sponza.world";
	const char* assetsPath = "phoenix.assets";

	World world;
	AssetRegistry assets;

	loadAssetRegistry(&assets, "phoenix.assets", renderDevice, renderContext);

	importTexture(g_defaultWhiteTexPath, nullptr, renderDevice, renderContext, &assets);
	importTexture(g_defaultBlackTexPath, nullptr, renderDevice, renderContext, &assets);

	addDefaultMaterial(&assets);

	LoadResources resources;
	resources.assets = &assets;
	resources.context = renderContext;
	resources.device = renderDevice;
	loadWorld(worldPath, &world, &resources);

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

		for (DirLightEntity& dl : world.m_dirLightEntities)
		{
			lightBuffer.addDirectional(viewTf * dl.m_dirLight.m_direction, dl.m_dirLight.m_color);
		}

		for (PointLightEntity& entity : world.m_pointLightEntities)
		{
			Vec4 eyePos(entity.m_transform.m_translation, 1.0);
			eyePos *= viewTf;

			addPointLightToBuffer(entity.m_pointLight, eyePos, &lightBuffer);
		}

		renderer.runLightsPass(lightBuffer);
		renderer.copyFinalColorToBackBuffer();

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		renderImGui();

		RI::swapBufferToWindow(gameWindow);
	}

	saveWorld(world, worldPath);
	saveAssetRegistry(assets, assetsPath);

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
