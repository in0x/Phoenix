#pragma once

#include <Core/EcsTypes.hpp>
#include <Memory/ChunkArray.hpp>

namespace Phoenix
{
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
		C* addComponent(EntityHandle handle, CtorArgs&&... ctorArgs)
		{
			Entity& entity = m_entities[handle];
			ComponentTypeId type = getComponentTypeId<C>();

			if (entity.hasComponent(type))
			{
				return nullptr;
			}

			ChunkArray<C>* pool = getComponentAllocator<C>();
			C* comp = pool->add(std::forward<CtorArgs>(ctorArgs)...);
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
}