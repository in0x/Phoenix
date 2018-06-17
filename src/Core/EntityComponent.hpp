#pragma once

#include <stdint.h>
#include <ECS/Entity.hpp>

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


		// Could feasibly generate all possible permutations (without order differences)

		// Easiset way that I know how to do this is like Unreal does its Delegates
		// Should split repeating snippets in these macros out into resuable expanded macros


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
}