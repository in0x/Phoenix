#pragma once

#include <stdint.h>
#include <assert.h>
#include <vector>
#include <stack>
#include <unordered_map>
#include <typeindex>

#include <Math/Vec3.hpp>
#include <Math/Matrix4.hpp>

namespace Phoenix
{
	class IComponent;

	template <class Type>
	constexpr std::type_index typeidx()
	{
		return std::type_index(typeid(Type));
	}

	typedef size_t EntityID;

	class Entity
	{
	public:
		template <class ComponentType>
		ComponentType* getComponent()
		{
			if (m_components.find(typeidx<ComponentType>()) != m_components.end())
			{
				return static_cast<ComponentType*>(m_components[typeidx<ComponentType>()]);
			}
			else
			{
				return nullptr;
			}
		}

		template <class ComponentType>
		void addComponent(ComponentType* component)
		{
			assert(m_components.find(typeidx<ComponentType>()) == m_components.end());
			m_components[typeidx<ComponentType>()] = component;
			component->m_owningEntity = this;
		}

		template <class ComponentType>
		void removeComponent(ComponentType* component)
		{
			assert(m_components.find(typeidx<ComponentType>()) != m_components.end());
			assert(m_components[typeidx<ComponentType>()] == component);
			m_components[typeidx<ComponentType>()] = nullptr;
			component->m_owningEntity = nullptr;
		}

		decltype(auto) begin()
		{
			return m_components.begin();
		}

		decltype(auto) end()
		{
			return m_components.end();
		}

	private:
		std::unordered_map<std::type_index, IComponent*> m_components;
	};

	class IComponent
	{
	public:
		virtual ~IComponent() {}

		virtual void init(IResource* resource) {}

		template <class ComponentType>
		ComponentType* sibling()
		{
			return m_owningEntity->getComponent<ComponentType>();
		}

		Entity* m_owningEntity;
	};

	struct World;

	class ISystem
	{
	public:
		virtual ~ISystem() {}
		virtual void update(World* world, float dt) = 0;
	};

	// Opaque resource type passed around to initialize component
	class IResource
	{
		virtual ~IResource() {}
	};

	// Possible structure
	// Components should be created and stored in the primary systems that tick them.
	// This ensures that they can be linearly accessed.
	//
	// Common components that are accessed in most systems should probably be base members of Entity.
	// We can figure which should be that later by profiling.
	//
	// Components that are rarely looked up by other systems can just be accessed via a GetComponent function.

	typedef IComponent*(*ComponentCreatorFptr)(IResource*);

	struct World
	{
		void registerSystem(ISystem* system)
		{
			m_systems.push_back(system);
		}

		template <class ComponentType>
		void registerComponentFactory(ComponentCreatorFptr creator)
		{
			assert(m_creatorFptrs.find(typeidx<ComponentType>()) == ComponentCreatorFptr.end());
			components[typeidx<ComponentType>()] = creator;
		}

		template <class ComponentType>
		ComponentType* createComponent(IResource* resource)
		{
			return static_cast<ComponentType>(m_creatorFptrs[typeidx<ComponentType>()](resource));
		}

		EntityID acquireEntity()
		{
			if (m_releasedEntities.size() > 0)
			{
				EntityID e = m_releasedEntities.top;
				m_releasedEntities.pop();
				return e;
			}
			else
			{
				m_entities.emplace_back();
				return m_entities.size() - 1;
			}
		}

		void releaseEntity(EntityID id)
		{
			Entity& e = m_entities[id];
			for (auto& pair : e)
			{
				IComponent* component = pair.second;
				e.removeComponent(component);
			}
		}

		std::vector<ISystem*> m_systems;

		std::vector<Entity> m_entities;

		std::stack<EntityID> m_releasedEntities; // Used to recycle entities

		std::unordered_map<std::type_index, ComponentCreatorFptr> m_creatorFptrs;
	};

	class Engine
	{
		void init();
		void exit();
		void update(float dt, World* world);
	};

	// Test impl

	class TransformComponent : public IComponent
	{
	public:
		Vec3 position;
		Vec3 rotation;
		Vec3 scale;
	};

	class DirectionalLightComponent : public IComponent 
	{
	public:
		Vec3 m_color;
		Vec3 m_direction;
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