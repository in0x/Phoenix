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
#include "Core/LoadResources.hpp"

#include "Core/Texture.hpp"
#include "Core/Material.hpp"

#include "Core/World.hpp"
#include "Core/Component.hpp"
#include "Core/Components/CTransform.hpp"
#include "Core/Components/CStaticMesh.hpp"

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
}

namespace Phoenix
{
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


	void objImportToWorld(const char* objPath, World* outworld, LoadResources* resources)
	{
		std::vector<StaticMesh*> import = importObj(objPath, resources->device, resources->context, resources->assets);

		for (StaticMesh* mesh : import)
		{
			EntityHandle entity = outworld->createEntity();
			CStaticMesh* sm = outworld->addComponent<CStaticMesh>(entity);
			CTransform* tf = outworld->addComponent<CTransform>(entity);

			sm->m_mesh = mesh;
			tf->setScale({ 0.1f, 0.1f, 0.1f });
		}
	}
}

namespace Phoenix
{
	typedef void CmdDrawFunc(void* cmd);
	typedef void CmdApplyFunc(void* cmd);

	struct EditorCmd
	{
		EditorCmd* nextCmd;
		void* payload;
		CmdApplyFunc* applyFunc;
	}; 

	typedef EditorCmd* CmdAllocFunc(StackAllocator& allocator, void* observedObj);

	struct CmdPayloadCTransform
	{
		CTransform* transform;
		float data[9];
		bool bWritten;
	};

	void applyEditorCmdCTransform(void* cmd)
	{
		EditorCmd* command = (EditorCmd*)cmd;
		CmdPayloadCTransform* payload = (CmdPayloadCTransform*)command->payload;

		if (!payload->bWritten)
		{
			return;
		}
		
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
		bool bWritten = false;

		bWritten |= ImGui::InputFloat3("Translation", &data[0], ImGuiInputTextFlags_EnterReturnsTrue);
		bWritten |= ImGui::InputFloat3("Rotation", &data[3], ImGuiInputTextFlags_EnterReturnsTrue);
		bWritten |= ImGui::InputFloat3("Scale", &data[6], ImGuiInputTextFlags_EnterReturnsTrue);

		payload->bWritten = bWritten;
	}

	EditorCmd* allocEditorCmdTransform(StackAllocator& allocator, void* observedObj)
	{
		EditorCmd* cmd = alloc<EditorCmd>(allocator);
		cmd->applyFunc = applyEditorCmdCTransform;
		
		CTransform* transform = (CTransform*)observedObj;
		CmdPayloadCTransform* payload = alloc<CmdPayloadCTransform>(allocator);
		payload->transform = transform;
		
		auto setDataFromVec = [](float* data, const Vec3& vec) {
			data[0] = vec.x;
			data[1] = vec.y;
			data[2] = vec.z;
		};

		setDataFromVec(payload->data, transform->getTranslation());
		setDataFromVec(&payload->data[3], transform->getRotation());
		setDataFromVec(&payload->data[6], transform->getScale());

		cmd->payload = payload;

		return cmd;
	}

	void drawEditorCmdNull(void* cmd) {}

	CmdDrawFunc* DrawCmdFuncTbl[ECType::CT_Max] =
	{
		&drawEditorCmdCTransform,
		&drawEditorCmdNull,
		&drawEditorCmdNull,
		&drawEditorCmdNull,
	};

	EditorCmd* allocEditorCmdNull(StackAllocator& allocator, void* observedObj) { return nullptr; }

	CmdAllocFunc* AllocCmdFuncTbl[ECType::CT_Max] =
	{
		&allocEditorCmdTransform,
		&allocEditorCmdNull,
		&allocEditorCmdNull,
		&allocEditorCmdNull,
	};

	class Inspector
	{
	public:
		StackAllocator m_allocator;
		EditorCmd* m_cmdhead;
		EditorCmd* m_cmdtail;
		EntityHandle m_selectedEntity;

		Inspector(size_t cmdMemoryBytes)
			: m_allocator(cmdMemoryBytes)
			, m_selectedEntity(0)
		{}	

		void drawDemoReference()
		{
			static bool checkBox = false;
			ImGui::Checkbox("Demo Window", &checkBox);

			if (checkBox)
			{
				ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
				ImGui::ShowDemoWindow(&checkBox);
			}
		}

		void drawEntityList(World* world)
		{	
			ImGui::Begin("World");
	
			const bool bCreatedEntity = ImGui::Button("Create Entity");
			if (bCreatedEntity)
			{
				world->createEntity();
			}

			for (size_t i = World::FIRST_VALID_ENTITY; i <= world->m_lastEntityIdx; ++i)
			{
				bool bSelected = ImGui::MenuItem("Entity");
				
				ImGui::SameLine();
				ImGui::Text("%lld", i);

				char bufCtx[32];
				sprintf(bufCtx, "Ctx%d", i);

				if (ImGui::BeginPopupContextWindow(bufCtx))
				{
					if (ImGui::Button("Destroy"))
					{
						//world->destroyEntity(m_selectedEntity);
					}

					ImGui::EndPopup();
				}
				else if (bSelected)
				{
					m_selectedEntity = i;
				}
			}

			if (bCreatedEntity)
			{
				m_selectedEntity = world->m_lastEntityIdx;
			}

			ImGui::End();
		}

		void applyEditorCommands()
		{
			while (m_cmdhead)
			{
				m_cmdhead->applyFunc(m_cmdhead);
				m_cmdhead = m_cmdhead->nextCmd;
			}
		}

		void clearEditorCommands()
		{
			m_allocator.clear();
			m_cmdhead = nullptr;
		}

		void insertCommand(EditorCmd* cmd)
		{
			if (!cmd)
			{
				return;
			}

			if (!m_cmdhead)
			{
				m_cmdhead = cmd;
				m_cmdtail = cmd;
			}

			m_cmdtail->nextCmd = cmd;
			m_cmdtail = cmd;
			cmd->nextCmd = nullptr;
		}

		void drawEntityEditor(World* world)
		{
			applyEditorCommands();
			clearEditorCommands();

			if (!world->handleIsValid(m_selectedEntity))
			{
				return;
			}

			Entity* entity = world->getEntity(m_selectedEntity);

			ImGui::Begin("Inspector");
			ImGui::Text("Entity %lld", m_selectedEntity);

			for (auto& kvp : entity->m_components)
			{
				Component* component = kvp.second;
				
				ImGui::BeginGroup();		
				ImGui::Text(component->typeName());

				CmdAllocFunc* allocCmd = AllocCmdFuncTbl[component->type()];
				EditorCmd* cmd = allocCmd(m_allocator, component);
				
				CmdDrawFunc* drawFunc = DrawCmdFuncTbl[component->type()];
				drawFunc(cmd);

				insertCommand(cmd);
				
				ImGui::EndGroup();
				ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 255, 255));
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

 	loadWorld(newWorldPath, &newWorld, &resources);
					  
	//objImportToWorld("Models/sponza/sponza.obj", &newWorld, &resources);
	//EntityHandle dirLightEntity = newWorld.createEntity();
	//CDirectionalLight* light = newWorld.addComponent<CDirectionalLight>(dirLightEntity);
	//light->m_color = Vec3(0.3f, 0.3f, 0.3f);
	//light->m_direction = Vec3(-0.5f, -0.5f, 0.f);

	const size_t editorCmdMemoryBytes = MEGABYTE(2);
	Inspector inspector(editorCmdMemoryBytes);
	
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

		inspector.drawEntityList(&newWorld);
		inspector.drawEntityEditor(&newWorld);

		inspector.drawDemoReference();

		renderImGui();

		RI::swapBufferToWindow(gameWindow);
	}

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
