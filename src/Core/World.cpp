#pragma once

#include "World.hpp"
#include <Core/Component.hpp>
#include <Core/Serialize.hpp>
#include <Core/SerialUtil.hpp>
#include <assert.h>

namespace Phoenix
{
	void World::addComponentFactory(ECType type, const ComponentFactory& factory)
	{
		auto& entry = m_factories.find(type);

		if (entry != m_factories.end())
		{
			assert(false);
			return;
		}

		m_factories.emplace(type, factory);
	}

	EntityHandle World::createEntity()
	{
		assert(m_lastEntityIdx < MAX_ENTITIES);
		return m_lastEntityIdx++;
	}

	bool World::handleIsValid(EntityHandle handle) const
	{
		return (handle != INVALID_ENTITY && m_lastEntityIdx < MAX_ENTITIES && handle <= m_lastEntityIdx);
	}

	Entity* World::getEntity(EntityHandle handle)
	{
		assert(handleIsValid(handle));
		return &m_entites[handle];
	}

	Component* World::addComponent(EntityHandle handle, ECType type)
	{
		assert(handleIsValid(handle));
		Entity& entity = m_entites[handle];

		ComponentFactory* factory = getCFactory(type);
		assert(factory != nullptr);

		Component* component = (*factory)();
		component->m_owner = handle;
		entity.addComponent(component);
		return component;
	}

	Component* World::getComponent(EntityHandle handle, ECType type)
	{
		assert(handleIsValid(handle));
		return m_entites[handle].getComponent(type);
	}
	
	void serialize(Archive* ar, ECType& ectype)
	{
		ar->serialize(&ectype, sizeof(ECType));
	}

	void saveEntityHeader(Entity* entity, WriteArchive* ar)
	{
		size_t numComponents = entity->m_components.size();
		serialize(ar, numComponents);

		for (auto& kvp : entity->m_components)
		{
			Component* component = kvp.second;
			ECType type = component->type();
			serialize(ar, type);
		}
	}

	void saveComponentData(Entity* entity, WriteArchive* ar)
	{
		for (auto& kvp : entity->m_components)
		{
			Component* component = kvp.second;
			component->save(ar);
		}
	}

	void saveWorld(World* world, const char* path)
	{
		WriteArchive ar;
		createWriteArchive(0, &ar);

		size_t numEntities = world->m_lastEntityIdx - 1;
		serialize(&ar, numEntities);

		for (size_t i = World::FIRST_VALID_ENTITY; i <= world->m_lastEntityIdx; ++i)
		{
			saveEntityHeader(world->getEntity(i), &ar);
			saveComponentData(world->getEntity(i), &ar);
		}

		EArchiveError err = writeArchiveToDisk(path, ar);
		assert(err == EArchiveError::NoError);
		destroyArchive(ar);
	}

	void addComponentsFromEntityHeader(ReadArchive* ar, EntityHandle eHandle, World* world)
	{
		size_t numComponents = 0;
		serialize(ar, numComponents);

		for (size_t i = 0; i < numComponents; ++i)
		{
			ECType type;
			serialize(ar, type);
			world->addComponent(eHandle, type);
		}
	}

	void loadComponentData(ReadArchive* ar, Entity* entity, LoadResources* resources)
	{
		for (auto& kvp : entity->m_components)
		{
			Component* component = kvp.second;
			component->load(ar, resources);
		}
	}

	void loadWorld(const char* path, World* outWorld, LoadResources* resources)
	{
		ReadArchive ar;
		EArchiveError err = createReadArchive(path, &ar);

		assert(err == EArchiveError::NoError);
		if (err != EArchiveError::NoError)
		{
			return;
		}

		size_t numEntitiesToLoad = 0;

		serialize(&ar, numEntitiesToLoad);

		for (size_t i = 0; i < numEntitiesToLoad; ++i)
		{
			EntityHandle eHandle = outWorld->createEntity();
			Entity* entity = outWorld->getEntity(eHandle);

			addComponentsFromEntityHeader(&ar, eHandle, outWorld);
			loadComponentData(&ar, entity, resources);
		}

		destroyArchive(ar);
	}
}