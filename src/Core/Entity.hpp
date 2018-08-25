#pragma once

#include <Core/EntityHandle.hpp>
#include <Core/ECType.hpp>
#include <unordered_map>

namespace Phoenix
{
	class Component;

	struct Entity
	{
		Component* getComponent(ECType type);
		void addComponent(Component* component);

		typedef std::unordered_map<ECType, Component*> ComponentTable;
		ComponentTable m_components;
	};
}