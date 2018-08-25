#pragma once

#include "Entity.hpp"
#include <Core/Component.hpp>

namespace Phoenix
{
	Component* Entity::getComponent(ECType type)
	{
		auto& entry = m_components.find(type);

		if (entry != m_components.end())
		{
			return entry->second;
		}
		else
		{
			return nullptr;
		}
	}

	void Entity::addComponent(Component* component)
	{
		ECType type = component->type();

		if (!getComponent(type))
		{
			m_components.emplace(type, component);
		}
	}
}