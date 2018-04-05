#pragma once

#include <limits>
#include <atomic>
#include <stdint.h>
#include <unordered_map>

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
}