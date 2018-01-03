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
			static_assert(is_derived_component<Comp>::value, "Registered component types need to inherit from Component");

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

			ChunkArray<Comp>* pool = getComponentPool<Comp>();
			Comp* comp = pool->add(std::forward(ctorArgs)...);
			comp->m_usingEntity = entityId;

			entity->registerComponent<Comp>(ComponentHandle(pool->size()));
			return comp;
		}

		template <typename Comp>
		Comp* getComponent(Entity::Id entityId, ComponentHandle compId)
		{
			Entity* entity = getEntity(entityId);
			assert(entity != nullptr);

			ChunkArray<Comp>* pool = getComponentPool<Comp>();
			Comp* comp = pool[compId];
			return comp;
		}

		template <typename Comp>
		void removeComponent(Entity::Id entityId)
		{
			Entity* entity = getEntity(entityId);
			entity->unregisterComponent<Comp>();
		}

		template <typename Comp>
		ChunkArray<Comp>* getComponentPool()
		{
			ComponentTypeId type = Comp::getType();
			assert(type < m_componentPools.size());
			return static_cast<ChunkArray<Comp>*>(m_componentPools[type].get());
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

		std::vector<Entity> m_entities;
		uint64_t m_entityIdCounter;
		std::unordered_map<Entity::Id, Entity*> m_entityMap;
		std::vector<std::unique_ptr<ChunkArrayBase>> m_componentPools;
	};

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

	private:
		World* m_world;
		size_t m_location;
		ComponentMask m_componentMask;
	};
}