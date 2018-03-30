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

//	namespace Components
//	{
//		enum Type
//		{
//			Transform,
//			StaticMesh,
//			Material,
//			DirectionalLight,
//			Camera,
//			Max
//		};
//
//		static_assert(Components::Max <= 64, "Currently only 64 unique component types are allowed");
//	};
//
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
			return m_components.find(type) != m_components.end();
		}

		std::unordered_map<ComponentTypeId, ComponentHandle> m_components;
	};

	struct World
	{
		~World()
		{
			for (ChunkArrayBase* allocator : m_componentAllocators)
			{
				delete allocator;
			}
		}

		World(World&) = delete;
		World(World&&) = delete;

		template <typename C>
		void registerComponentType(size_t initialCapacity = 64)
		{
			ComponentTypeId componentType = getComponentTypeId<C>();

			if (m_componentAllocators.capacity() < componentType)
			{
				m_componentAllocators.resize(componentType);
				ChunkArray<C>* pool = new ChunkArray<Comp>(initialCapacity);
				m_componentPools.push_back(pool);
			}
		}

		ComponentHandle getComponentHandle(ComponentTypeId componentType, EntityHandle entityId)
		{
			Entity& entity = m_entities[entityId];
			
			if (!entity.hasComponent(componentType))
			{
				return invalidHandle();
			}

			return entity.m_components.at(componentType);
		}
		
		void* getComponent(ComponentTypeId componentType, EntityHandle entityId)
		{
			ComponentHandle handle = getComponentHandle(componentType, entityId);
			
			if (handle == invalidHandle())
			{
				return nullptr;
			}

			return m_componentAllocators[componentType]->at(handle);
		}

		void removeComponent(ComponentTypeId componentType, EntityHandle entityId)
		{
			// TODO: deregister from entity
			ComponentHandle handle = getComponentHandle(componentType, entityId);
			
			if (handle == invalidHandle())
			{
				return;
			}
			
			ChunkArrayBase* allocator = m_componentAllocators[componentType];
			allocator->swapAndPop(handle);
		}

	private:
		std::vector<Entity> m_entities;
		std::vector<ChunkArrayBase*> m_componentAllocators;
		uint64_t m_numEntities;
	};
	
	class Component
	{
	public:
		template <typename C>
		C* sibling()
		{
			return m_owningWorld->getComponent(getComponentTypeId<C>(), m_owningEntity);
		}

	private:
		World* m_owningWorld;
		EntityHandle m_owningEntity;
	};

	template <typename C>
	class ComponentIterator
	{
	public:
		ComponentIterator(World* world);

		ComponentIterator begin();
		ComponentIterator end();

		bool operator ==(const ComponentIterator<C>& other);
		bool operator !=(const ComponentIterator<C>& other) {return !(*this == other)};

		void operator++();

		C& operator*();
	};

	class ISystem
	{
	public:
		ISystem(World* world) 
			: m_world(world)
		{}
		
		virtual ~ISystem() {}
		virtual void tick(float dt) = 0;
	
	protected:
		World* m_world;
	};

	struct CDirectionalLight : public Component
	{
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

	class DrawStaticMeshSystem : public ISystem
	{
		virtual void tick(float dt) override
		{
			for (CStaticMesh& mesh : ComponentIterator<CStaticMesh>(m_world))
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
		getComponentTypeId<CDirectionalLight>();

		World world;

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


