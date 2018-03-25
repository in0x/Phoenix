#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <assert.h>

#include "Entity.hpp"
#include <Memory/ChunkArray.hpp>

namespace Phoenix
{
	struct World
	{
		World()
			: m_entityIdCounter(0)
		{}

		template <typename Comp>
		void registerComponentType()
		{
			ComponentTypeId type = Comp::getType();

			if (m_componentPools.size() <= type)
			{
				auto pool = std::make_unique<ChunkArray<Comp>>(128);
				m_componentPools.push_back(std::move(pool));
			}
		}

		template <typename Comp, typename ...CtorArgs>
		Comp* addComponent(Entity::Id entityId, CtorArgs... ctorArgs)
		{
			Entity* entity = getEntity(entityId);
			assert(entity != nullptr);
			assert(!entity->hasComponent<Comp>());

			ChunkArray<Comp>& pool = getComponentPool<Comp>();
			Comp* comp = pool.add(std::forward(ctorArgs)...);
			comp->m_usingEntity = entityId;
			comp->m_worldIn = this;

			entity->registerComponent(ComponentHandle(pool.size()), Comp::getType(), Comp::getMask());
			return comp;
		}

		template <typename Comp>
		Comp* getComponent(Entity::Id entityId)
		{
			Entity* entity = getEntity(entityId);
			assert(entity != nullptr);

			if (entity->hasComponent<Comp>())
			{
				ChunkArray<Comp>& pool = getComponentPool<Comp>();
				ComponentHandle handle = entity->m_componentIds[Comp::getType()];
				Comp* comp = pool[handle.m_id];
				return comp;
			}
			else
			{
				return nullptr;
			}
		}

		template <typename Comp>
		void removeComponent(Entity::Id entityId)
		{
			Entity* entity = getEntity(entityId);
			entity->unregisterComponent(Comp::getType(), Comp::getMask());
		}

		template <typename Comp, typename ...CtorArgs>
		Comp* addSingletonComponent(CtorArgs... ctorArgs)
		{
			ComponentTypeId type = Comp::getType();

			if (!m_singletonComponents.find(type))
			{
				m_singletonComponents.emplace(type, std::make_unique<Comp>(std::forward(ctorArgs)));
				return m_singletonComponents[type].get();
			}
			else
			{
				return nullptr;
			}
		}

		template <typename Comp>
		Comp* getSingletonComponent()
		{
			if (m_singletonComponents.find(type))
			{
				return m_singletonComponents[type].get();
			}
			else
			{
				return nullptr;
			}
		}

		Entity::Id createEntity()
		{
			// TODO: Check if we can reuse any entities

			Entity::Id id = m_entityIdCounter++;
			assert(id < std::numeric_limits<Entity::Id>::max());

			m_entities.emplace_back(id, this);
			m_entityMap.emplace(id, &m_entities.back());

			return id;
		}

		void destroyEntity(Entity::Id entityId)
		{
			Entity* entity = getEntity(entityId);
			entity->reset();
			m_entityMap[entityId] = nullptr;
			// TODO: return the entity to reusable list
		}

		Entity* getEntity(Entity::Id entityId)
		{
			assert(entityId < m_entityIdCounter);
			assert(m_entityMap.find(entityId) != m_entityMap.end());
			return m_entityMap[entityId];
		}

		template <typename Comp>
		ChunkArray<Comp>& getComponentPool()
		{
			ComponentTypeId type = Comp::getType();
			assert(type < m_componentPools.size());
			return *static_cast<ChunkArray<Comp>*>(m_componentPools[type].get());
		}

		std::unordered_map<Entity::Id, Entity*> m_entityMap;
		std::unordered_map<ComponentTypeId, std::unique_ptr<ComponentBase>> m_singletonComponents;

		std::vector<Entity> m_entities;
		std::vector<std::unique_ptr<ChunkArrayBase>> m_componentPools;
		
		uint64_t m_entityIdCounter;
	};

	// Iterates over all components in the world, skipping all that do not match the comp signature.
	// Needing to use this is probably a bad idea and indicates that the system is too complex.
	template <typename ...Components>
	class WorldIterator
	{
	public:
		WorldIterator(World* world)
			: m_componentMask(0)
			, m_location(0)
		{
			[&](...) {}((m_componentMask |= Components::getMask())...);
		}

		void operator++()
		{
			while (!m_world->m_entities[m_location].hasComponents<Components...>() && m_location < m_world->m_entityIdCounter)
			{
				m_location++;
			}
		}

		Entity& operator*()
		{
			return m_world->m_entities[m_location];
		}

		Entity* operator->()
		{
			return &m_world->m_entities[m_location];
		}

		explicit operator bool() const
		{
			m_location < m_world->m_entityIdCounter;
		}

	private:
		World* m_world;
		size_t m_location;
		ComponentMask m_componentMask;
	};

	// Iterates all Comps in the world. Use Sibling<T>() to get component of another type.
	template <typename Comp>
	class ComponentIterator
	{
	public:
		ComponentIterator(World* world)
			: m_location(0)
		{
			m_componentPool = world->getComponentPool<Component>();
		}

		void operator++()
		{
			if (m_location < m_componentPool->size())
			{
				m_location++;
			}
		}

		Comp& operator*()
		{
			return m_componentPool[m_location];
		}

		Comp* operator->()
		{
			return &m_componentPool[m_location];
		}

		explicit operator bool() const
		{
			m_location < m_componentPool->size();
		}

	private:
		ChunkArray<Component>* m_componentPool;
		size_t m_location;
	};
}