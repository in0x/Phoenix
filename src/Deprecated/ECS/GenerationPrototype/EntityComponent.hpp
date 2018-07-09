#pragma once

#include <stdint.h>
#include <ECS/Entity.hpp>
#include <Memory/PoolAllocator.hpp>

namespace Phoenix
{
	/*
	Define EntityType by set of components
	Entity Types are enum
	EntityType can be represented as enum bitmask (64 is fine for now)
	All entities of one EntityType are stored in contiguous storage (like The Machinery)
	Systems define a set of components they are interested in
	When updating systems, we run over all types and give system the entity
	containers that match the components it is interested in
	*/

	// struct GeneratedEntity : public Entity
	// {
	// 	CTransformComponent m_transform;
	// 	CHealthComponent m_health;
	//
	// 	virtual uint64_t getComponentMask() const override
	// 	{
	// 		return EComponent::Transform | EComponent::Health;
	// 	}
	//
	// 	virtual void* getComponent(uint64_t type) override
	// 	{
	// 		// Could maybe be faster if we could figure out a way to add a lookup table indexed with type
	// 		switch (type)
	// 		{
	// 		case EComponent::Transform:
	// 		{
	// 			return &m_transform;
	// 		}
	// 		case EComponent::Health:
	// 		{
	// 			return &m_health;
	// 		}
	// 		default:
	// 		{
	// 			return nullptr;
	// 		}
	// 		}
	// 	}
	// };

	/*
	Big issue: Add/RemoveComponent requires being able to safely
	copy component data between Entity types. But memory order of components
	is not always going to be the same.

	Example

	Entity: [Transform, Mesh, Health]

	| RemoveComponent(EComponent::Mesh)

	Entity: [Transform, Health]

	Cant memcpy because there is a gap between Transform and Health in src data
	*/

	void copyEntity()
	{
		// Could probably copy by iterarting all component types,
		// checking if both entities have it, 
		// if yes, do e2->GetComponent<C>() = e1->GetComponent<C>()
		// Only problem is how do I iterate types
	}

	namespace Tests
	{
		void runEcsTests()
		{
			//TestTransformComponentTestHealthComponentEntity e;

			return;
		}
	}

	class IEntityContainer
	{
	public:
		virtual Entity* allocateEntity() = 0;	
		virtual void Entity* freeEntity(Entity* entity) override;	
	};

	template <typename TEntity>
	class TEntityContainer : public IEntityContainer
	{
	public:
		TEntityContainer()
			: m_allocator(sizeof(TEntity), 1024, alignof(TEntity))
		{}

		virtual Entity* allocateEntity() override
		{
			return m_allocator.alloc();
		}

		virtual void Entity* freeEntity(Entity* entity) override
		{
			m_allocator.free(entity);
		}

	private:
		PoolAllocator m_allocator;
	}

	void initEntityInfo(EntityInfo* info)
	{
		// Generate code to setup all info here
	}

	struct EntityInfo
	{
		size_t sizeOfEntityPool(uint64_t entityMask)
		{
			auto iter = m_poolSizes.find(entityMask);

			if (iter == m_poolSizes.end())
			{
				return 0;
			} 

			return iter->second;
		}

		std::unordered_map<uint64_t, size_t> m_poolSizes;		
	}
}