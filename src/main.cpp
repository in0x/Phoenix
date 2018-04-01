#include <assert.h>

#include "Tests/MathTests.hpp"
#include "Tests/MemoryTests.hpp"

#include "Core/Logger.hpp"
#include "Core/Windows/PlatformWindows.hpp"

#include "Math/PhiMath.hpp"

#include "Render/RIOpenGL/RIOpenGL.hpp"
#include "Render/DeferredRenderer.hpp"

#include "Core/Mesh.hpp"
#include "Core/Material.hpp"
#include "Memory/ChunkArray.hpp"

#include <unordered_map>
#include <atomic>

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

//	struct CCamera
//	{
//		CCamera(float horizontalFOV, float screenWidth, float screenHeight, float near, float far)
//			: m_horizontalFOV(horizontalFOV)
//			, m_screenWidth(screenWidth)
//			, m_screenHeight(screenHeight)
//			, m_near(near)
//			, m_far(far)
//		{}
//
//		Matrix4 getProjection()
//		{
//			return perspectiveRH(m_horizontalFOV, m_screenWidth / m_screenHeight, m_near, m_far);
//		}
//
//		float m_horizontalFOV;
//		float m_screenWidth;
//		float m_screenHeight;
//		float m_near;
//		float m_far;
//	};

namespace Phoenix
{
	typedef uint64_t ComponentTypeId;
	typedef uint64_t ComponentHandle;
	typedef uint64_t EntityHandle; 
	
	uint64_t invalidHandle() // Consider bringing back Handle type.
	{
		return std::numeric_limits<uint64_t>::max();
	}

	struct ComponentTypeIdGenerator
	{
		static ComponentTypeId NextId()
		{
			return s_ComponentTypeIdCounter++;
		}

	private:
		static std::atomic<ComponentTypeId> s_ComponentTypeIdCounter;
	};

	std::atomic<ComponentTypeId> ComponentTypeIdGenerator::s_ComponentTypeIdCounter = 0;

	template <typename C>
	ComponentTypeId getComponentTypeId()
	{
		static ComponentTypeId thisId = ComponentTypeIdGenerator::NextId();
		return thisId;
	}

	struct Entity
	{
		bool hasComponent(ComponentTypeId type)
		{
			return m_components.find(type) != m_components.end() && m_components[type] != invalidHandle();
		}

		std::unordered_map<ComponentTypeId, ComponentHandle> m_components;
	};

	struct World
	{
		World() = default;
		~World();
		World(World&) = delete;
		World(World&&) = delete;

		void* getComponent(ComponentTypeId type, EntityHandle entityId);
		bool isComponentTypeRegistered(ComponentTypeId type);
		EntityHandle createEntity();

		template <typename C>
		void registerComponentType(size_t initialCapacity = 64)
		{
			static_assert(std::is_base_of<Component, C>::value, "Entity components need to derive from Component.");

			ComponentTypeId componentType = getComponentTypeId<C>();

			if (isComponentTypeRegistered(componentType))
			{
				return;
			}

			m_componentAllocators.resize(componentType + 1);
			ChunkArray<C>* pool = new ChunkArray<C>(initialCapacity);
			m_componentAllocators[componentType] = pool;
		}

		template <typename C, typename ...CtorArgs>
		C* addComponent(EntityHandle handle, CtorArgs... ctorArgs)
		{
			Entity& entity = m_entities[handle];
			ComponentTypeId type = getComponentTypeId<C>();

			if (entity.hasComponent(type))
			{
				return nullptr;
			}

			ChunkArray<C>* pool = getComponentAllocator<C>();
			C* comp = pool->add(ctorArgs...);
			comp->m_world = this;
			comp->m_entity = handle;

			entity.m_components.emplace(type, handle);

			return comp;
		}

		template <class C>
		void removeComponent(EntityHandle entityId)
		{
			ComponentTypeId type = getComponentTypeId<C>();
			ComponentHandle handle = getComponentHandle(type, entityId);

			if (handle == invalidHandle())
			{
				return;
			}

			Entity& entity = m_entities[entityId];
			entity.m_components[componentType] = invalidHandle();

			ChunkArray<C>* allocator = getComponentAllocator<C>();
			allocator->swapAndPop(handle);

			// Since we swapped the removed component with the most recently alloced, 
			// we need to patch the handle map in the entity owning the swapped component. 
			Component* swappedComp = (Component*)allocator->at(handle);
			Entity& owningEntity = m_entities[swappedComp->m_entity];
			owningEntity.m_components[componentType] = handle;
		}


	private:
		template <typename C>
		friend class ComponentIterator;

		template <typename C>
		ChunkArray<C>* getComponentAllocator()
		{
			ComponentTypeId type = getComponentTypeId<C>();
			
			if (!isComponentTypeRegistered(type))
			{
				return nullptr;
			}

			ChunkArrayBase* baseAlloc = m_componentAllocators[type];
			return (ChunkArray<C>*)baseAlloc;
		}

		ComponentHandle getComponentHandle(ComponentTypeId componentType, EntityHandle entityId);
		
		std::vector<Entity> m_entities;
		std::vector<ChunkArrayBase*> m_componentAllocators;
		uint64_t m_numEntities;
	};

	struct Component
	{
		template <typename C>
		C* sibling()
		{
			return static_cast<C*>(m_world->getComponent(getComponentTypeId<C>(), m_entity));
		}

		World* m_world;
		EntityHandle m_entity;
	};

	World::~World()
	{
		for (ChunkArrayBase* allocator : m_componentAllocators)
		{
			delete allocator;
		}
	}

	ComponentHandle World::getComponentHandle(ComponentTypeId componentType, EntityHandle entityId)
	{
		Entity& entity = m_entities[entityId];

		if (!entity.hasComponent(componentType))
		{
			return invalidHandle();
		}

		return entity.m_components.at(componentType);
	}

	void* World::getComponent(ComponentTypeId componentType, EntityHandle entityId)
	{
		ComponentHandle handle = getComponentHandle(componentType, entityId);

		if (handle == invalidHandle())
		{
			return nullptr;
		}

		return m_componentAllocators[componentType]->at(handle);
	}

	bool World::isComponentTypeRegistered(ComponentTypeId type)
	{
		return type < m_componentAllocators.size();
	}

	EntityHandle World::createEntity()
	{
		if (m_numEntities >= invalidHandle())
		{
			return invalidHandle(); 
		}

		m_entities.emplace_back();
		return (EntityHandle)(m_entities.size() - 1);
	}

	template <typename C>
	class ComponentIterator : public ChunkArrayIterator<C> 
	{
	public:
		ComponentIterator(World* world) : ChunkArrayIterator(nullptr)
		{
			m_arr = world->getComponentAllocator<C>();
		}
	};

	struct CDirectionalLight : public Component
	{
	public:
		CDirectionalLight(const Vec3& direction, const Vec3& color)
			: m_direction(direction)
			, m_color(color)
		{}

		Vec3 m_direction;
		Vec3 m_color;
	};

	struct CTransform : public Component
	{
		CTransform()
			: m_translation(0.f, 0.f, 0.f)
			, m_rotation(0.f, 0.f, 0.f)
			, m_scale(1.f, 1.f, 1.f)
		{}

		Vec3 m_translation;
		Vec3 m_scale;
		Vec3 m_rotation;

		Matrix4 toMat4() const
		{
			return Matrix4::translation(m_translation.x, m_translation.y, m_translation.z)
				* Matrix4::rotation(m_rotation.x, m_rotation.y, m_rotation.z)
				* Matrix4::scale(m_scale.x, m_scale.y, m_scale.z);
		}
	};

	struct CStaticMesh : public Component
	{
		CStaticMesh() = default;

		CStaticMesh(const RenderMesh& mesh, const Material& material)
			: m_mesh(mesh)
			, m_material(material)
		{}

		RenderMesh m_mesh;
		Material m_material;
	};

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

	RIOpenGL renderInterface;
	bool bRIstarted = renderInterface.init();

	if (!bRIstarted)
	{
		Logger::error("Failed to initialize RenderInterface");
		assert(false);
	}

	IRIDevice* renderDevice = renderInterface.getRenderDevice();
	IRIContext* renderContext = renderInterface.getRenderContex();
	renderContext->setDepthTest(EDepth::Enable);

	WindowConfig config;
	config.width = 800;
	config.height = 600;
	config.title = "Phoenix";
	config.bFullscreen = false;

	std::unique_ptr<RenderWindow> windowPtr = renderInterface.createWindow(config);

	RenderWindow* window = windowPtr.get();

	renderInterface.setWindowToRenderTo(window);

	DeferredRenderer renderer(renderDevice, renderContext, config.width, config.height);
	
	Matrix4 viewTf = lookAtRH(Vec3(0.f, 0.f, 7.f), Vec3(0.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
	renderer.setViewMatrix(viewTf);

	Matrix4 projTf = perspectiveRH(70.f, (float)config.width / (float)config.height, 0.1f, 100.f);
	renderer.setProjectionMatrix(projTf);
	
	World world;
	world.registerComponentType<CDirectionalLight>();
	world.registerComponentType<CStaticMesh>();
	world.registerComponentType<CTransform>();

	RenderMesh foxMesh = loadRenderMesh("Models/Fox/RedFox.obj", renderDevice);
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

	while (!window->wantsToClose())
	{
		rotator.tick(&world, 0);
		drawMeshSystem.tick(&world, 0);
		dirLightSystem.tick(&world, 0);

		renderContext->endPass();

		renderInterface.swapBufferToWindow(window);

		Platform::pollEvents();
	}

	renderInterface.exit();
	Logger::exit();
	return 0;
}


