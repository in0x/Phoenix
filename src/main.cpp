#include <assert.h>

#include "Tests/MathTests.hpp"
#include "Tests/MemoryTests.hpp"

#include "Core/Logger.hpp"
#include "Core/Windows/PlatformWindows.hpp"

#include "Math/PhiMath.hpp"

#include "Render/RIOpenGL/RIOpenGL.hpp"
#include "Render/DeferredRenderer.hpp"

#include "Core/Mesh.hpp"
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
//
//	struct CTransform
//	{
//		Vec3 m_translation;
//		Vec3 m_scale;
//		Vec3 m_rotation;
//
//		Matrix4 toMat4() const
//		{
//			return Matrix4::translation(m_translation.x, m_translation.y, m_translation.z)
//				* Matrix4::rotation(m_rotation.x, m_rotation.y, m_rotation.z)
//				* Matrix4::scale(m_scale.x, m_scale.y, m_scale.z);
//		}
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
		void removeComponent(ComponentTypeId type, EntityHandle entityId);
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

			ChunkArray<C>* pool = (ChunkArray<C>*)(m_componentAllocators[type]);
			C* comp = pool->add(std::forward(ctorArgs)...);
			comp->m_world = this;
			comp->m_entity = handle;

			entity.m_components.emplace(type, handle);

			return comp;
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

	void World::removeComponent(ComponentTypeId componentType, EntityHandle entityId)
	{
		ComponentHandle handle = getComponentHandle(componentType, entityId);

		if (handle == invalidHandle())
		{
			return;
		}

		Entity& entity = m_entities[entityId];
		entity.m_components[componentType] = invalidHandle();

		ChunkArrayBase* allocator = m_componentAllocators[componentType];
		allocator->swapAndPop(handle);

		// Since we swapped the removed component with the most recently alloced, 
		// we need to patch the handle map in the entity owning the swapped component. 
		Component* swappedComp = (Component*)allocator->at(handle);
		Entity& owningEntity = m_entities[swappedComp->m_entity];
		owningEntity.m_components[componentType] = handle;
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

	class CDirectionalLight : public Component
	{
	public:
		CDirectionalLight() = default;

		Vec3 m_direction;
		Vec3 m_color;
	};

	struct CStaticMesh : public Component
	{
		RenderMesh m_mesh;
	};

	struct CMaterial : public Component
	{
		Vec3 m_diffuse;
		Vec3 m_specular;
		float m_specularExp;
	};

	struct ISystem
	{
		virtual ~ISystem() {}
		virtual void tick(World* world, float dt) = 0;
	};

	struct DrawStaticMeshSystem : public ISystem
	{
		virtual void tick(World* world, float dt) override
		{
			for (CStaticMesh& mesh : ComponentIterator<CStaticMesh>(world))
			{
				CMaterial* material = mesh.sibling<CMaterial>();
				// draw(mesh, material);
			}
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

	RIOpenGL renderInterface;
	bool bRIstarted = renderInterface.init();

	if (!bRIstarted)
	{
		Logger::error("Failed to initialize RenderInterface");
		assert(false);
	}

	IRIDevice* renderDevice = renderInterface.getRenderDevice();
	IRIContext* renderContext = renderInterface.getRenderContex();

	WindowConfig config;
	config.width = 800;
	config.height = 600;
	config.title = "Phoenix";
	config.bFullscreen = false;

	std::unique_ptr<RenderWindow> windowPtr = renderInterface.createWindow(config);

	RenderWindow* window = windowPtr.get();

	renderInterface.setWindowToRenderTo(window);

	// ECS Test
	{
		World world;

		world.registerComponentType<CDirectionalLight>();
		world.registerComponentType<CStaticMesh>();

		EntityHandle e = world.createEntity();
	
		world.addComponent<CDirectionalLight>(e);

		DrawStaticMeshSystem system;

		system.tick(&world, 0);
	}

	while (!window->wantsToClose())
	{
		//renderer.setupGBufferPass(renderContext);
		//renderer.fillGBuffer(entity, renderContext);

		//renderer.setupLightPass(renderContext);
		//renderer.drawLight(lightRed, renderContext);
		//renderer.drawLight(lightBlue, renderContext);
		//renderer.drawLight(lightGreen, renderContext);

		renderContext->endPass();

		renderInterface.swapBufferToWindow(window);

		Platform::pollEvents();
	}

	renderInterface.exit();
	Logger::exit();
	return 0;
}


