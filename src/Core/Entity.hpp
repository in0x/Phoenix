#pragma once

#include <stdint.h>

#include "Component.hpp"

namespace Phoenix
{
	struct World;

	struct Entity
	{
		typedef uint64_t Id;

		Entity(Id id, World* world)
			: m_id(id)
			, m_worldIn(world)
			, m_componentMask(0)
		{}

		void registerComponent(ComponentHandle handle, ComponentTypeId type, ComponentMask mask)
		{
			assert(m_componentIds.find(type) == m_componentIds.end());

			m_componentIds.emplace(type, handle);
			m_componentMask |= mask;
		}

		void unregisterComponent(ComponentTypeId type, ComponentMask mask)
		{
			assert(m_componentIds.find(type) != m_componentIds.end());

			m_componentIds[type].invalidate();
			m_componentMask &= ~(1i64 << mask);
		}

		template <typename Comp>
		bool hasComponent()
		{
			return (m_componentMask & Comp::getMask()) == Comp::getMask();
		}

		template <typename ...Comps>
		bool hasComponents()
		{
			ComponentMask mask = 0;
			[&](...) {}((mask |= Comps::getMask())...);
			return (m_componentMask & mask) == mask;
		}

		void reset()
		{
			m_componentMask = 0;
			m_componentIds.clear();
		}

		Id m_id;
		World* m_worldIn;
		ComponentMask m_componentMask;
		std::unordered_map<ComponentTypeId, ComponentHandle> m_componentIds;
	};
}

