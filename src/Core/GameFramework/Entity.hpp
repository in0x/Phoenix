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

		template <typename Comp>
		void registerComponent(ComponentHandle handle)
		{
			assert(m_componentIds.find(Comp::getType()) == m_componentIds.end());

			m_componentIds.emplace(Comp::getType(), handle);
			m_componentMask |= Comp::getMask();

		}

		template <typename Comp>
		void unregisterComponent()
		{
			assert(m_componentIds.find(Comp::getType()) != m_componentIds.end());

			m_componentIds[Comp::getType()].invalidate();
			m_componentMask &= ~(1i64 << Comp::getMask());
		}

		template <typename Comp, typename ...CtorArgs>
		Comp* addComponent(CtorArgs... ctorArgs)
		{
			return m_worldIn->addComponent<Comp, CtorArgs...>(m_id, std::forward(ctorArgs)...);
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

		template <typename Comp>
		Comp* getComponent()
		{
			if (hasComponent<Comp>())
			{
				return m_worldIn->getComponent<Comp>(m_id, m_componentIds[Comp::getType()]);
			}
			else
			{
				return nullptr;
			}
		}

		template <typename Comp>
		void removeComponent()
		{
			m_worldIn->removeComponent<Comp>(m_id);
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

