#pragma once

#include <stdint.h>
#include <assert.h>
#include <vector>
#include <unordered_map>

#include <Math/Vec3.hpp>
#include <Math/Matrix4.hpp>

namespace Phoenix::ECS	
{
	typedef size_t ComponentID;
	typedef size_t EntityID;

	class IComponent;

	class Entity
	{
	public:
		template <class ComponentType>
		ComponentType* getComponent()
		{
			if (components.find(&typeid(ComponentType)) != components::end())
			{
				return static_cast<ComponentType>(components[&typeid(ComponentType)]);
			}
			else
			{
				return nullptr;
			}
		}

		template <class ComponentType>
		void addComponent(ComponentType* component)
		{
			assert(components.find(&typeid(ComponentType)) == components::end());
			components[&typeid(ComponentType)] = component;
		}

		template <class ComponentType>
		void removeComponent(ComponentType* component)
		{
			assert(components.find(&typeid(ComponentType)) == components::end());
			components[&typeid(ComponentType)] = nullptr;
		}

	private:
		std::unordered_map<const std::type_info*, IComponent*> components;
	};

	namespace Phoenix
	{
		class IComponent
		{
			virtual ~IComponent();

			template <class ComponentType>
			ComponentType* sibling()
			{
				return m_owningEntity->getComponent<ComponentType>();
			}

		private:
			Entity* m_owningEntity;
		};
	}
	
	class World;

	class ISystem
	{
		virtual ~ISystem();
		void update(World* world, float dt);
	};

	// Opaque resource type passed around to initialize component
	class IResource
	{
		virtual ~IResource();
	};

	// Possible structure
	// Components should be created and stored in the primary systems that tick them.
	// This ensures that they can be linearly accessed.
	//
	// Common components that are accessed in most systems should probably be base members of Entity.
	// We can figure which should be that later by profiling.
	//
	// Components that are rarely looked up by other systems can just be accessed via a GetComponent function.

	struct World
	{
		std::vector<Entity> entities;
		std::vector<ISystem*> systems;

		void registerSystem(ISystem* system);

		// Use template and type_id trickery to register component factories. 
		// Can use typeid of template type as key to hashmap.
		// https://gamedev.stackexchange.com/questions/55950/how-can-i-properly-access-the-components-in-my-c-entity-component-systems

		template <class ComponentType>
		ComponentType* createComponent(IResource* resource); 

		template<class ComponentType>
		void registerComponentFactory(ComponentType*(*factoryFunc)(IResource*));
	};

	class Engine
	{
		void init();
		void exit();
		void update(float dt, World* world);
	};

	// overwatch

	//class EntityAdmin
	//{
	//	array<System*>
	//	hash_map<EntityID, Entity*>
	//	object_pool<Component>*
	//	array<Component*>*
	//};

	//class System 
	//{
	//	virtual ~System();
	//	void Update(f32);
	//	void NotifyComponent(Component*)
	//};

	//class Entity
	//{
	//	EntityID
	//	array<Component*>
	//	resourceHandle
	//};

	//class Component
	//{
	//	virtual ~Component();
	//	void Create(resource*)
	//};

	//void SomeSystem::Update(EntityAdmin* world)
	//{
	//	for (SomeComponent* c : ComponentItr<SomeComponent>(world))
	//	{
	//		doAthing(c, c->Sibling<AnotherComponent>())
	//	}
	//}
}