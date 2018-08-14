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

#include "Memory/StackAllocator.hpp"

#include "Render/DeferredRenderer.hpp"
#include "Render/LightBuffer.hpp"

#include "UI/PhiImGui.h"

namespace Phoenix
{
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
}

namespace Phoenix
{
	enum ECType
	{
		CT_Transform,
		CT_StaticMesh,
		CT_PointLight,
		CT_DirectionalLight
	};

	#define IMPL_EC_TYPE_ID(typeEnum, typeNamePretty) \
	static ECType staticType() \
	{ \
		return typeEnum; \
	} \
	\
	virtual ECType type() const override \
	{ \
		return typeEnum; \
	} \
	\
	virtual const char* typeName() const override \
	{ \
		return typeNamePretty; \
	} \

	typedef int32_t EntityHandle;

	class Component
	{
	public:
		Component() : m_owner(0) {}

		virtual ECType type() const = 0;
		virtual const char* typeName() const = 0;
		
		virtual void save(Archive* ar) = 0;
		virtual void load(Archive* ar, LoadResources* resources) = 0;

		EntityHandle m_owner;
	};

	struct Entity
	{
		Component* getComponent(ECType type);
		void addComponent(Component* component);

		typedef std::unordered_map<ECType, Component*> ComponentTable;
		ComponentTable m_components;
	};

	Component* Entity::getComponent(ECType type)
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
		ECType type = component->type();

		if (!getComponent(type))
		{
			m_components.emplace(type, component);
		}
	}

	typedef std::function<Component*(void)> ComponentFactory;

	typedef std::unordered_map<ECType, ComponentFactory> CFactoryTable;

	class World
	{
	public:
		enum
		{
			MAX_ENTITIES = 1024,
			INVALID_ENTITY = 0,
			FIRST_VALID_ENTITY = 1
		};

		World()
			: m_lastEntityIdx(1) {}

		EntityHandle createEntity();
		bool handleIsValid(EntityHandle handle) const;
		Entity* getEntity(EntityHandle handle);
		
		void addComponentFactory(ECType type, const ComponentFactory& factory);

		Component* addComponent(EntityHandle handle, ECType type);
		Component* getComponent(EntityHandle handle, ECType type);

		template<typename C>
		C* addComponent(EntityHandle handle)
		{
			return static_cast<C*>(addComponent(handle, C::staticType()));
		}

		template<typename C>
		C* getComponent(EntityHandle handle)
		{
			return static_cast<C*>(getComponent(handle, C::staticType()));
		}

		Entity m_entites[MAX_ENTITIES];
		size_t m_lastEntityIdx;

	private:
		ComponentFactory* getCFactory(ECType type)
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

	void World::addComponentFactory(ECType type, const ComponentFactory& factory)
	{
		auto& entry = m_factories.find(type);

		if (entry != m_factories.end())
		{
			assert(false);
			return;
		}

		m_factories.emplace(type, factory);
	}

	EntityHandle World::createEntity()
	{
		assert(m_lastEntityIdx < MAX_ENTITIES);
		return ++m_lastEntityIdx;
	}

	bool World::handleIsValid(EntityHandle handle) const
	{
		return (handle != INVALID_ENTITY && m_lastEntityIdx < MAX_ENTITIES);
	}

	Entity* World::getEntity(EntityHandle handle)
	{
		assert(handleIsValid(handle));
		return &m_entites[handle];
	}

	Component* World::addComponent(EntityHandle handle, ECType type)
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

	Component* World::getComponent(EntityHandle handle, ECType type)
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

		void serial(Archive* ar)
		{
			serialize(ar, m_translation);
			serialize(ar, m_rotation);
			serialize(ar, m_scale);
		}

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

		virtual void save(Archive* ar) override
		{
			serial(ar);
		}

		virtual void load(Archive* ar, LoadResources* resources) override
		{
			serial(ar);
			m_bDirty = true;
		}

		Matrix4* m_transform;

		bool m_bDirty;

		IMPL_EC_TYPE_ID(ECType::CT_Transform, "Transform");
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

		virtual void save(Archive* ar) override
		{
			serialize(ar, m_mesh->m_name);
		}

		virtual void load(Archive* ar, LoadResources* resources) override
		{
			std::string meshName;
			serialize(ar, meshName);
			m_mesh = loadStaticMesh(meshName.c_str(), resources->device, resources->context, resources->assets);
		}

		IMPL_EC_TYPE_ID(ECType::CT_StaticMesh, "StaticMesh");
	};

	class StaticMeshSystem
	{
	public:
		Component* allocComponent();
		void renderMeshes(World* world, DeferredRenderer* renderer);

	private:
		enum { MAX_COMPONENTS = 1024 };
		ComponentArray<CStaticMesh, MAX_COMPONENTS> m_components;
	};

	Component* StaticMeshSystem::allocComponent()
	{
		return m_components.alloc();
	}
	
	void StaticMeshSystem::renderMeshes(World* world, DeferredRenderer* renderer)
	{
		const size_t active = m_components.m_active;
		
		for (size_t i = 0; i < active; ++i)
		{
			CStaticMesh& sm = m_components[i];
			CTransform* tf = world->getComponent<CTransform>(sm.m_owner);

			renderer->drawStaticMesh(*sm.m_mesh, *tf->m_transform);
		}
	}

	class CDirectionalLight : public Component
	{
	public:
		Vec3 m_direction;
		Vec3 m_color;

		virtual void save(Archive* ar) override
		{
			serialize(ar, m_direction);
			serialize(ar, m_color);
		}

		virtual void load(Archive* ar, LoadResources* resources) override
		{
			serialize(ar, m_direction);
			serialize(ar, m_color);
		}

		IMPL_EC_TYPE_ID(ECType::CT_DirectionalLight, "DirectionalLight")
	};

	class CPointLight : public Component
	{
	public:
		Vec3 m_color;
		float m_radius;
		float m_intensity;


		virtual void save(Archive* ar) override
		{
			serialize(ar, m_color);
			serialize(ar, m_radius);
			serialize(ar, m_intensity);
		}

		virtual void load(Archive* ar, LoadResources* resources) override
		{
			serialize(ar, m_color);
			serialize(ar, m_radius);
			serialize(ar, m_intensity);
		}

		IMPL_EC_TYPE_ID(ECType::CT_PointLight, "PointLight")
	};

	class LightSystem
	{
	public:

		Component* allocDirLight();	
		Component* allocPointLight();

		void renderLights(World* world, DeferredRenderer* renderer, const Matrix4& viewTf);

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

	void LightSystem::renderLights(World* world, DeferredRenderer* renderer, const Matrix4& viewTf)
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

	void serialize(Archive* ar, ECType& ectype)
	{
		ar->serialize(&ectype, sizeof(ECType));
	}

	void saveEntityHeader(Entity* entity, WriteArchive* ar)
	{
		size_t numComponents = entity->m_components.size();
		serialize(ar, numComponents);

		for (auto& kvp : entity->m_components)
		{
			Component* component = kvp.second;
			ECType type = component->type();
			serialize(ar, type);
		}
	}

	void saveComponentData(Entity* entity, WriteArchive* ar)
	{
		for (auto& kvp : entity->m_components)
		{
			Component* component = kvp.second;
			component->save(ar);
		}
	}

	void saveWorld(World* world, const char* path)
	{
		WriteArchive ar;
		createWriteArchive(0, &ar);

		size_t numEntities = world->m_lastEntityIdx - 1;
		serialize(&ar, numEntities);

		for (size_t i = World::FIRST_VALID_ENTITY; i < world->m_lastEntityIdx; ++i)
		{
			saveEntityHeader(&world->m_entites[i], &ar);
			saveComponentData(&world->m_entites[i], &ar);
		}

		EArchiveError err = writeArchiveToDisk(path, ar);
		assert(err == EArchiveError::NoError);
		destroyArchive(ar);
	}

	void addComponentsFromEntityHeader(ReadArchive* ar, EntityHandle eHandle, World* world)
	{
		size_t numComponents = 0;	
		serialize(ar, numComponents);

		for (size_t i = 0; i < numComponents; ++i)
		{
			ECType type;
			serialize(ar, type);
			world->addComponent(eHandle, type);
		}
	}

	void loadComponentData(ReadArchive* ar, Entity* entity, LoadResources* resources)
	{
		for (auto& kvp : entity->m_components)
		{
			Component* component = kvp.second;
			component->load(ar, resources);
		}
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

		size_t numEntitiesToLoad = 0;

		serialize(&ar, numEntitiesToLoad);

		for (size_t i = 0; i < numEntitiesToLoad; ++i)
		{
			EntityHandle eHandle = outWorld->createEntity();
			Entity* entity = &outWorld->m_entites[eHandle];
			
			addComponentsFromEntityHeader(&ar, eHandle, outWorld);
			loadComponentData(&ar, entity, resources);
		}

		destroyArchive(ar);
	}
}

namespace Phoenix
{
	typedef void DrawFunc(void*);
	typedef void ApplyFunc(void*);

	struct EditorCmd
	{
		void* nextCmd;
		void* payload;
		DrawFunc* drawFunc;
		ApplyFunc* applyFunc;
	}; 

	struct CmdPayloadCTransform
	{
		CTransform* transform;
		float data[9];
	};

	void applyEditorCmdCTransform(void* cmd)
	{
		EditorCmd* command = (EditorCmd*)cmd;
		CmdPayloadCTransform* payload = (CmdPayloadCTransform*)command->payload;
		float* data = payload->data;

		payload->transform->setTranslation({ data[0], data[1], data[2] });
		payload->transform->setRotation({ data[3], data[4], data[5] });
		payload->transform->setScale({ data[6], data[7], data[8] });
	}

	void drawEditorCmdCTransform(void* cmd)
	{
		EditorCmd* command = (EditorCmd*)cmd;
		CmdPayloadCTransform* payload = (CmdPayloadCTransform*)command->payload;
		float* data = payload->data;

		ImGui::InputFloat3("Translation", &data[0], 1);
		ImGui::InputFloat3("Rotation", &data[3], 1);
		ImGui::InputFloat3("Scale", &data[6], 1);
	}

	void allocEditorCmd(EditorCmd* prev, StackAllocator& allocator, CTransform* transform)
	{
		EditorCmd* cmd = alloc<EditorCmd>(allocator);
		cmd->payload = alloc<CmdPayloadCTransform>(allocator);
		cmd->applyFunc = applyEditorCmdCTransform;
		cmd->drawFunc = drawEditorCmdCTransform;
		prev->nextCmd = cmd;
	}

	class Inspector
	{
	public:
		StackAllocator m_allocator;
		EditorCmd* m_cmdhead;

		Inspector(size_t cmdMemoryBytes)
			: m_allocator(cmdMemoryBytes)
			, m_cmdhead(nullptr)
		{}	

		void drawWorld(World* world)
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

			ImGui::MenuItem("VectorTest");
	
			for (size_t i = World::FIRST_VALID_ENTITY; i < world->m_lastEntityIdx; ++i)
			{
				Entity* entity = world->getEntity(i);

				ImGui::MenuItem("Entity", nullptr, false, false);
				ImGui::Text("Entity %d", i);

				for (auto& kvp : entity->m_components)
				{
					Component* component = kvp.second;
					ImGui::Text(component->typeName());
				}
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
	const char* newWorldPath = "Assets/sponzanew.world";
	const char* assetsPath = "phoenix.assets";

	AssetRegistry assets;

	loadAssetRegistry(&assets, "phoenix.assets", renderDevice, renderContext);

	importTexture(g_defaultWhiteTexPath, nullptr, renderDevice, renderContext, &assets);
	importTexture(g_defaultBlackTexPath, nullptr, renderDevice, renderContext, &assets);

	addDefaultMaterial(&assets);

	LoadResources resources;
	resources.assets = &assets;
	resources.context = renderContext;
	resources.device = renderDevice;
	
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

	World newWorld;

	auto transformFactory = [system = &tfSystem]() -> Component*
	{
		return system->allocComponent();
	};
	newWorld.addComponentFactory(CTransform::staticType(), transformFactory);

	auto smFactory = [system = &smSystem]() -> Component*
	{
		return system->allocComponent();
	};
	newWorld.addComponentFactory(CStaticMesh::staticType(), smFactory);

	auto dlFactory = [system = &lightSystem]() -> Component*
	{
		return system->allocDirLight();
	};
	newWorld.addComponentFactory(CDirectionalLight::staticType(), dlFactory);

	auto plFactory = [system = &lightSystem]() -> Component*
	{
		return system->allocPointLight();
	};
	newWorld.addComponentFactory(CPointLight::staticType(), plFactory);

	///// NEW ECS INIT - END /////

	loadWorld(newWorldPath, &newWorld, &resources);
	
	const size_t editorCmdMemoryBytes = 2048;
	Inspector inspector(editorCmdMemoryBytes);

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

	saveWorld(&newWorld, newWorldPath);

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
