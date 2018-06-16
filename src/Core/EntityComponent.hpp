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

	struct EComponent
	{
		static const uint64_t Transform = 1 << 0;
		static const uint64_t Health = 1 << 1;
	};

	struct TestTransformComponent
	{
		static const uint64_t s_type = EComponent::Transform;

		float x;
		float y;
		float z;
	};

	struct TestHealthComponent
	{
		static const uint64_t s_type = EComponent::Health;

		float health;
	};

	struct GeneratedEntity : public Entity
	{
		TestTransformComponent m_transform;
		TestHealthComponent m_health;

		virtual uint64_t getComponentMask() const override
		{
			return EComponent::Transform | EComponent::Health;
		}

		virtual void* getComponent(uint64_t type) override
		{
			// Could maybe be faster if we could figure out a way to add a lookup table indexed with type
			switch (type)
			{
			case EComponent::Transform:
			{
				return &m_transform;
			}

			case EComponent::Health:
			{
				return &m_health;
			}

			default:
			{
				return nullptr;
			}
			}
		}
	};

	// Generates ^^^^
	//IMPL_ENTITY(TransformComponent, m_transform, HealthComponent, m_health) // Do we want/need a typename?

#define EXPAND_HELPER(x) #x
#define EXPAND(x) EXPAND_HELPER(x)

#define PHI_COMPONENT_1_NAME(C_NAME) C_NAME ## Entity

#define PHI_COMPONENT_2_NAME(C_NAME1, C_NAME2) C_NAME1 ## C_NAME2 ## Entity

#define PHI_COMPONENT_1_MEMBER(MEMBER_TYPE, MEMBER_NAME) MEMBER_TYPE MEMBER_NAME ; 

#define PHI_COMPONENT_2_MEMBER(MEMBER_TYPE_1, MEMBER_NAME_1, MEMBER_TYPE_2, MEMBER_NAME_2) \
	PHI_COMPONENT_1_MEMBER(MEMBER_TYPE_1, MEMBER_NAME_1) \
	PHI_COMPONENT_1_MEMBER(MEMBER_TYPE_2, MEMBER_NAME_2) \

#define PHI_COMPONENT_1_MASK(C_TYPE) C_TYPE ## ::s_type \

#define PHI_COMPONENT_2_MASK(C_TYPE1, C_TYPE2) C_TYPE1 ## ::s_type | C_TYPE2 ## ::s_type

#define PHI_COMPONENT_1_GET(C_TYPE, C_NAME) \
	case C_TYPE ## ::s_type: \
	{ \
		return &C_NAME; \
	} \

#define PHI_COMPONENT_2_GET(C_TYPE1, C_NAME1, C_TYPE2, C_NAME2) \
	PHI_COMPONENT_1_GET(C_TYPE1, C_NAME1) \
	PHI_COMPONENT_1_GET(C_TYPE2, C_NAME2) \

#define IMPL_ENTITY(ENTITY_NAME, MEMBER_DECL, MASK_DECL, GET_DECL) \
	struct ENTITY_NAME : public Entity \
	{ \
		MEMBER_DECL \
		\
		virtual uint64_t getComponentMask() const override \
		{ \
			return MASK_DECL; \
		} \
		virtual void* getComponent(uint64_t type) override \
		{ \
			switch (type) \
			{ \
				GET_DECL \
				default: \
				{ \
					return nullptr; \
				} \
			} \
		} \
	}; \

#define PHI_ENTITY_TWO_COMPONENTS(TYPE1, NAME1, TYPE2, NAME2) \
	IMPL_ENTITY(PHI_COMPONENT_2_NAME(TYPE1, TYPE2), PHI_COMPONENT_2_MEMBER(TYPE1, NAME1, TYPE2, NAME2), PHI_COMPONENT_2_MASK(TYPE1, TYPE2), PHI_COMPONENT_2_GET(TYPE1, NAME1, TYPE2, NAME2)) \

	//PHI_ENTITY_TWO_COMPONENTS(TestTransformComponent, m_transform, TestHealthComponent, m_health)

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