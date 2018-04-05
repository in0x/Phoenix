#pragma once

#include <Core/World.hpp>

namespace Phoenix
{
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

	template <typename C>
	class ComponentIterator : public ChunkArrayIterator<C>
	{
	public:
		ComponentIterator(World* world) : ChunkArrayIterator(nullptr)
		{
			m_arr = world->getComponentAllocator<C>();
		}
	};
}