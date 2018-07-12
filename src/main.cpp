#include <assert.h>
#include <chrono>
#include <typeindex>
#include <functional>

#include "Tests/MathTests.hpp"
#include "Tests/MemoryTests.hpp"
#include "Render/RIOpenGL/RIOpenGL.hpp"

#include "Core/ObjImport.hpp"
#include "Core/AssetRegistry.hpp"
#include "Core/FNVHash.hpp"

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

	struct DirectionalLight
	{
		Vec3 m_direction;
		Vec3 m_color;
	};

	struct PointLight
	{
		Vec3 m_color;
		float m_radius;
		float m_intensity;
	};

	struct StaticMeshEntity 
	{
		StaticMesh* m_mesh;
		Transform m_transform;
	};

	struct DirLightEntity
	{
		DirectionalLight m_dirLight;
		Transform m_transform;
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
	typedef FNVHash EcTypeId;
	#define EC_TYPE_HASH(x) HashFNV<const char*>()(x);
	
	// TODO(phil): We can make this compile-time. 
	#define IMPL_EC_TYPE_ID(x) \
	static EcTypeId staticTypeId() \
	{ \
		return EC_TYPE_HASH(x); \
	} \
	\
	virtual EcTypeId typeId() const override \
	{ \
		return EC_TYPE_HASH(x); \
	} \
	\
	virtual const char* typeName() const override \
	{ \
		return x; \
	} \

	typedef int32_t EntityHandle;

	class Component
	{
	public:
		Component() : m_owner(0) {}

		virtual EcTypeId typeId() const = 0;
		virtual const char* typeName() const = 0;
		EntityHandle m_owner;
	};

	class Entity
	{
	public:
		Component* getComponent(EcTypeId type);
		void addComponent(Component* component);
	
	private:
		typedef std::unordered_map<EcTypeId, Component*> ComponentTable;
		ComponentTable m_components;
	};

	Component* Entity::getComponent(EcTypeId type)
	{
		auto& entry = m_components.find(type);
		
		if (entry != m_components.end())
		{
			return entry->second;
		}
		else
		{
			return nullptr;
		}
	}

	void Entity::addComponent(Component* component)
	{
		EcTypeId type = component->typeId();

		if (!getComponent(type))
		{
			m_components.emplace(type, component);
		}
	}

	typedef std::function<Component*(void)> ComponentFactory;

	typedef std::unordered_map<EcTypeId, ComponentFactory> CFactoryTable;

	class NewWorld
	{
		enum
		{
			MAX_ENTITIES = 1024,
			INVALID_ENTITY = 0,
			FIRST_VALID_ENTITY = 1
		};

	public:
		NewWorld()
			: m_usedEntities(1) {}

		EntityHandle createEntity();
		bool handleIsValid(EntityHandle handle);

		void addComponentFactory(EcTypeId type, const ComponentFactory& factory);

		Component* addComponent(EntityHandle handle, EcTypeId type);
		Component* getComponent(EntityHandle handle, EcTypeId type);

		template<typename C>
		C* addComponent(EntityHandle handle)
		{
			return static_cast<C*>(addComponent(handle, C::staticTypeId()));
		}

		template<typename C>
		C* getComponent(EntityHandle handle)
		{
			return static_cast<C*>(getComponent(handle, C::staticTypeId()));
		}

		Entity m_entites[MAX_ENTITIES];
		size_t m_usedEntities;

	private:
		ComponentFactory* getCFactory(EcTypeId type)
		{
			auto& entry = m_factories.find(type);

			if (entry != m_factories.end())
			{
				return &(entry->second);
			}
			else
			{
				return nullptr;
			}
		}

		CFactoryTable m_factories;
	};

	void NewWorld::addComponentFactory(EcTypeId type, const ComponentFactory& factory)
	{
		auto& entry = m_factories.find(type);

		if (entry != m_factories.end())
		{
			assert(false);
			return;
		}

		m_factories.emplace(type, factory);
	}

	EntityHandle NewWorld::createEntity()
	{
		assert(m_usedEntities < MAX_ENTITIES);
		return ++m_usedEntities;
	}

	bool NewWorld::handleIsValid(EntityHandle handle)
	{
		return (handle != INVALID_ENTITY && m_usedEntities < MAX_ENTITIES);
	}

	Component* NewWorld::addComponent(EntityHandle handle, EcTypeId type)
	{
		assert(handleIsValid(handle));
		Entity& entity = m_entites[handle];

		ComponentFactory* factory = getCFactory(type);
		assert(factory != nullptr);

		Component* component = (*factory)();
		component->m_owner = handle;
		entity.addComponent(component);
		return component;
	}

	Component* NewWorld::getComponent(EntityHandle handle, EcTypeId type)
	{
		assert(handleIsValid(handle));
		return m_entites[handle].getComponent(type);
	}

	template <typename C, size_t maxComponents>
	struct ComponentArray
	{
		ComponentArray() : m_active(0) {}

		C m_components[maxComponents];
		size_t m_active;

		C* alloc()
		{
			assert(m_active < maxComponents);
			if (m_active >= maxComponents)
			{
				return nullptr;
			}

			return &m_components[m_active++];
		}

		C& operator[](size_t index)
		{
			return m_components[index];
		}
	};
	
	class CTransform : public Component
	{
		Vec3 m_translation;
		Vec3 m_rotation;
		Vec3 m_scale;

	public:
		void setTranslation(const Vec3& t)
		{
			m_translation = t;
			m_bDirty = true;
		}

		const Vec3& getTranslation()
		{
			return m_translation;
		}

		void setRotation(const Vec3& r)
		{
			m_rotation = r;
			m_bDirty = true;
		}

		const Vec3& getRotation()
		{
			return m_rotation;
		}

		void setScale(const Vec3& s)
		{
			m_scale = s;
			m_bDirty = true;
		}

		const Vec3& getScale()
		{
			return m_scale;
		}

		Matrix4* m_transform;

		bool m_bDirty;

		IMPL_EC_TYPE_ID("Transform");
	};

	class TransformSystem
	{
	public:
		TransformSystem();

		void updateTransforms();

		Component* allocComponent();

	private:
		enum { MAX_COMPONENTS = 1024 };

		ComponentArray<CTransform, MAX_COMPONENTS> m_components;
		Matrix4 m_transforms[MAX_COMPONENTS];
	};

	TransformSystem::TransformSystem()
	{
		for (size_t i = 0; i < MAX_COMPONENTS; ++i)
		{
			m_components[i].m_transform = &m_transforms[i];
		}
	}

	Component* TransformSystem::allocComponent()
	{
		return m_components.alloc();
	}

	void TransformSystem::updateTransforms()
	{
		const size_t active = m_components.m_active;
		
		for (size_t i = 0; i < active; ++i)
		{
			CTransform& c = m_components[i];

			if (c.m_bDirty)
			{
				m_transforms[i] = Matrix4::translation(c.getTranslation())
								* Matrix4::rotation(c.getRotation())
								* Matrix4::scale(c.getScale());

				c.m_bDirty = false;
			}
		}
	}

	class CStaticMesh : public Component
	{
	public:
		CStaticMesh() : m_mesh(nullptr) {}

		StaticMesh* m_mesh;

		IMPL_EC_TYPE_ID("StaticMesh");
	};

	class StaticMeshSystem
	{
	public:
		Component* allocComponent();
		void renderMeshes(NewWorld* world, DeferredRenderer* renderer);

	private:
		enum { MAX_COMPONENTS = 1024 };
		ComponentArray<CStaticMesh, MAX_COMPONENTS> m_components;
	};

	Component* StaticMeshSystem::allocComponent()
	{
		return m_components.alloc();
	}
	
	void StaticMeshSystem::renderMeshes(NewWorld* world, DeferredRenderer* renderer)
	{
		const size_t active = m_components.m_active;
		
		for (size_t i = 0; i < active; ++i)
		{
			CStaticMesh& sm = m_components[i];
			CTransform* tf = world->getComponent<CTransform>(sm.m_owner);

			renderer->drawStaticMesh(*sm.m_mesh, *tf->m_transform);
		}
	}

	struct CDirectionalLight : public Component
	{
		Vec3 m_direction;
		Vec3 m_color;

		IMPL_EC_TYPE_ID("DirectionalLight")
	};

	struct CPointLight : public Component
	{
		Vec3 m_color;
		float m_radius;
		float m_intensity;

		IMPL_EC_TYPE_ID("PointLight")
	};

	class LightSystem
	{
	public:

		Component* allocDirLight();	
		Component* allocPointLight();

		void renderLights(NewWorld* world, DeferredRenderer* renderer, const Matrix4& viewTf);

	private:
		enum { MAX_COMPONENTS = 256 };

		ComponentArray<CDirectionalLight, MAX_COMPONENTS> m_dirLights;
		ComponentArray<CPointLight, MAX_COMPONENTS> m_pointLights;
		LightBuffer m_lightBuffer;
	};

	Component* LightSystem::allocDirLight()
	{
		return m_dirLights.alloc();
	}
	
	Component* LightSystem::allocPointLight()
	{
		return m_pointLights.alloc();
	}

	void LightSystem::renderLights(NewWorld* world, DeferredRenderer* renderer, const Matrix4& viewTf)
	{
		renderer->setupDirectLightingPass();
		m_lightBuffer.clear();

		for (size_t i = 0; i < m_dirLights.m_active; ++i)
		{
			CDirectionalLight& dl = m_dirLights[i];
			m_lightBuffer.addDirectional(viewTf * dl.m_direction, dl.m_color);
		}

		for (size_t i = 0; i < m_pointLights.m_active; ++i)
		{
			CPointLight& pl = m_pointLights[i];
			CTransform* tf = world->getComponent<CTransform>(pl.m_owner);

			Vec4 eyePos(tf->getTranslation(), 1.0);
			eyePos *= viewTf;

			m_lightBuffer.addPointLight(eyePos, pl.m_radius, pl.m_color, pl.m_intensity);
		}

		renderer->runLightsPass(m_lightBuffer);
	}

	void convertTransformOldToNew(const Transform& oldTf, CTransform* newTf)
	{
		newTf->setTranslation(oldTf.m_translation);
		newTf->setRotation(oldTf.m_rotation);
		newTf->setScale(oldTf.m_scale);
	}

	void convertWorldOldToNew(World* oldWorld, NewWorld* newWorld)
	{
		for (const StaticMeshEntity& entity : oldWorld->m_staticMeshEntities)
		{
			EntityHandle e = newWorld->createEntity();
			
			CStaticMesh* sm = newWorld->addComponent<CStaticMesh>(e);
			sm->m_mesh = entity.m_mesh;

			CTransform* tf = newWorld->addComponent<CTransform>(e);
			convertTransformOldToNew(entity.m_transform, tf);
		}

		for (const DirLightEntity& entity : oldWorld->m_dirLightEntities)
		{
			EntityHandle e = newWorld->createEntity();

			CDirectionalLight* dl = newWorld->addComponent<CDirectionalLight>(e);
			dl->m_color = entity.m_dirLight.m_color;
			dl->m_direction = entity.m_dirLight.m_direction;
		}
	
		for (const PointLightEntity& entity : oldWorld->m_pointLightEntities)
		{
			EntityHandle e = newWorld->createEntity();
		
			CPointLight* pl = newWorld->addComponent<CPointLight>(e);
			pl->m_color = entity.m_pointLight.m_color;
			pl->m_intensity = entity.m_pointLight.m_intensity;
			pl->m_radius = entity.m_pointLight.m_radius;

			CTransform* tf = newWorld->addComponent<CTransform>(e);
			convertTransformOldToNew(entity.m_transform, tf);
		}	
	}
}

namespace Phoenix
{
	class Inspector
	{
	public:
		void drawWorld(NewWorld* world)
		{			
			static bool checkBox = false;

			//ImGui::Text("Hello, world!");
			//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
			//ImGui::ColorEdit3("clear color", (float*)&editColor);

			ImGui::Checkbox("Demo Window", &checkBox);

			//if (ImGui::Button("Button"))
			//{
			//	counter++;
			//}

			//ImGui::SameLine();
			//ImGui::Text("counter = %d", counter);

			//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			if (checkBox)
			{
				ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); 
				ImGui::ShowDemoWindow(&checkBox);
			}		

			ImGui::Begin("Inspector");

			const size_t numEntities = world->m_usedEntities;

			for (size_t i = 0; i < numEntities; ++i)
			{
				ImGui::MenuItem("Entity", NULL, false, false);
				ImGui::Text("Entity %d", i);
			}

			ImGui::End();
		}
	};
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

	///// NEW ECS INIT - START /////

	TransformSystem tfSystem;
	StaticMeshSystem smSystem;
	LightSystem lightSystem;

	NewWorld newWorld;

	auto transformFactory = [system = &tfSystem]() -> Component*
	{
		return system->allocComponent();
	};
	newWorld.addComponentFactory(CTransform::staticTypeId(), transformFactory);

	auto smFactory = [system = &smSystem]() -> Component*
	{
		return system->allocComponent();
	};
	newWorld.addComponentFactory(CStaticMesh::staticTypeId(), smFactory);

	auto dlFactory = [system = &lightSystem]() -> Component*
	{
		return system->allocDirLight();
	};
	newWorld.addComponentFactory(CDirectionalLight::staticTypeId(), dlFactory);

	auto plFactory = [system = &lightSystem]() -> Component*
	{
		return system->allocPointLight();
	};
	newWorld.addComponentFactory(CPointLight::staticTypeId(), plFactory);

	EntityHandle e = newWorld.createEntity();
	newWorld.addComponent(e, CTransform::staticTypeId());

	///// NEW ECS INIT - END /////

	convertWorldOldToNew(&world, &newWorld);

	Inspector inspector;

	///// NEW ECS UPDATE - START /////

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

		tfSystem.updateTransforms();

		Matrix4 viewTf = camera.getUpdatedViewMatrix();
		renderer.setViewMatrix(viewTf);
		renderer.setupGBufferPass();

		smSystem.renderMeshes(&newWorld, &renderer);
		lightSystem.renderLights(&newWorld, &renderer, viewTf);
	
		renderer.copyFinalColorToBackBuffer();

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		inspector.drawWorld(&newWorld);

		renderImGui();

		RI::swapBufferToWindow(gameWindow);
	}

	///// NEW ECS UPDATE - END /////

	//saveWorld(world, worldPath);
	//saveAssetRegistry(assets, assetsPath);

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
