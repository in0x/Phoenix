#pragma once

#include <Core/EntityHandle.hpp>
#include <Core/ECType.hpp>
#include <Core/Entity.hpp>

#include <unordered_map>
#include <functional>

namespace Phoenix
{
	class Component;
	struct Entity;
	struct LoadResources;

	typedef std::function<Component*(void)> ComponentFactory;

	typedef std::unordered_map<ECType, ComponentFactory> CFactoryTable;

	class World
	{
	public:
		enum
		{
			MAX_ENTITIES = 1024,
			INVALID_ENTITY = 0,
			FIRST_VALID_ENTITY = 1
		};

		World()
			: m_lastEntityIdx(1) {}

		EntityHandle createEntity();
		bool handleIsValid(EntityHandle handle) const;
		Entity* getEntity(EntityHandle handle);

		void addComponentFactory(ECType type, const ComponentFactory& factory);

		Component* addComponent(EntityHandle handle, ECType type);
		Component* getComponent(EntityHandle handle, ECType type);

		template<typename C>
		C* addComponent(EntityHandle handle)
		{
			return static_cast<C*>(addComponent(handle, C::staticType()));
		}

		template<typename C>
		C* getComponent(EntityHandle handle)
		{
			return static_cast<C*>(getComponent(handle, C::staticType()));
		}

		Entity m_entites[MAX_ENTITIES];
		size_t m_lastEntityIdx;

	private:
		ComponentFactory* getCFactory(ECType type)
		{
			auto& entry = m_factories.find(type);

			if (entry != m_factories.end())
			{
				return &(entry->second);
			}
			else
			{
				return nullptr;
			}
		}

		CFactoryTable m_factories;
	};

	void saveWorld(World* world, const char* path);
	void loadWorld(const char* path, World* outWorld, LoadResources* resources);
}
