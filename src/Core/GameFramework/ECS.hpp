#pragma once

#include <stdint.h>
#include <assert.h>
#include <vector>
#include <unordered_map>
#include <memory>

#include <Memory/StackAllocator.hpp>

namespace Phoenix
{
	// Basic typedef for 64 different kinds of components now, 
	// can be abstracted if needed to support e.g. larger array
	// for more bits
	typedef uint64_t ComponentMask;
	typedef size_t ComponentTypeId;
	typedef size_t ComponentId;

	//template <class Type>
	//constexpr std::type_index typeidx()
	//{
	//	return std::type_index(typeid(Type));
	//}

	struct World;

	struct Entity
	{
		typedef size_t Id;

		Entity(World* world) {}

		Id m_id;

		ComponentMask m_componentMask;

		World* m_worldIn;
	};

	class ISystem
	{
	public:
		virtual ~ISystem() {}
		virtual void tick(World* world, float dt) = 0;
	};

	struct ComponentBase
	{
		enum
		{
			MaxComponentTypes = (sizeof(ComponentMask) * CHAR_BIT)
		};

		static size_t s_componentTypeCounter;
	};

	uint64_t ComponentBase::s_componentTypeCounter = 0;

	template <typename DerivedComponent>
	struct Component
	{
		static ComponentTypeId getType() // Type can be used to lookup eg object pools in world
		{
			static ComponentBase::Type type = ComponentBase::s_componentTypeCounter++;
			assert(typeCount < ComponentBase::MaxComponentTypes);
			return type;
		}

		static ComponentMask getMask()
		{
			static ComponentMask mask = 1 << getType();
			return mask;
		}
	};

	template <typename T>
	struct is_derived_component
	{
		template <typename U> static std::true_type check(decltype(U::getMask)*);
		template <typename> static std::false_type check(...);

		typedef decltype(check<T>(0)) result;

	public:
		static const bool value = result::value;
	};

	// Opaque resource type passed around to initialize component
	class IResource
	{
		virtual ~IResource() {}
	};

	// TODO: Ensure alignment of first alloc per chunk. 
	class ChunkArrayBase
	{
	public:
		ChunkArrayBase(size_t elemSizeBytes, size_t chunkSizeBytes)
			: m_sizePerAllocBytes(elemSizeBytes)
			, m_chunkSizeBytes(chunkSizeBytes)
			, m_lastAllocIdx(0)
		{
		}

		ChunkArrayBase(size_t initialCapacity, size_t elemSizeBytes, size_t chunkSizeBytes)
			: m_sizePerAllocBytes(elemSizeBytes)
			, m_chunkSizeBytes(chunkSizeBytes)
			, m_lastAllocIdx(0)
		{
			expandTo(initialCapacity);
		}

		void expandTo(size_t numElements)
		{
			while (capacity() < numElements)
			{
				m_chunks.push_back(new StackAllocator(m_chunkSizeBytes));
			}
		}

		size_t capacity() const 
		{
			return (m_chunkSizeBytes / m_sizePerAllocBytes) * m_chunks.size();
		}

		void* add()
		{

		}

		void* operator[](size_t idx)
		{
		
		}

	private:
		std::vector<StackAllocator*> m_chunks;
		size_t m_sizePerAllocBytes;
		size_t m_chunkSizeBytes;
		size_t m_lastAllocIdx;
	};

	
	template<typename T>
	class ChunkArray : public ChunkArrayBase
	{
	public:
		class ChunkArray(size_t elementsPerChunk) 
			: ChunkArrayBase(sizeof(T), elementsPerChunk * sizeof(T))
		{}

		class ChunkArray(size_t initialCapacity, size_t elementsPerChunk)
			: ChunkArrayBase(initialCapacity, sizeof(T), elementsPerChunk * sizeof(T))
		{}	
	};

	struct World {};

	//struct World
	//{
	//	template <typename Comp>
	//	void registerComponentType()
	//	{
	//		static_assert(is_derived_component<Comp>::value, "Registered component types need to inherit from Component<T>");

	//		ComponentTypeId type = Comp::getType();

	//		if (m_componentPools.size() < type)
	//		{
	//			auto pool = std::make_unique<ChunckedPool<Comp>>();
	//			m_componentPools.push_back(pool);
	//		}
	//	}

	//	template <typename Comp>
	//	Comp* createComponent(Entity::Id entityId)
	//	{
	//		Entity* entity = getEntity(entityId);
	//		assert(entity != nullptr);

	//		ChunckedPool<Comp>* pool = getComponentPool<Comp>();
	//		// TODO: allocate component
	//		// TODO: register the component on the entity
	//		// TODO: return the component
	//	}

	//	template <typename Comp>
	//	ChunckedPool<Comp>* getComponentPool()
	//	{
	//		ComponentTypeId type =  ComponentTypeId::getType();
	//		assert(m_componentPools.size() < type);
	//		return static_cast<ChunckedPool<Comp>*>(m_componentPools[type].get());
	//	}

	//	Entity::Id createEntity()
	//	{
	//		// TODO: Check if we can reuse any components
	//		// TODO: if not, allocate a new one, creating 
	//		// the id by incrementing a running counter
	//	}

	//	void destroyEntity(Entity::Id entityId)
	//	{
	//		// TODO: Release all owned components
	//		// TODO: return the entity to reusable list
	//	}

	//	Entity* getEntity(Entity::Id entityId)
	//	{
	//		if (m_entityMap.find(entityId) != m_entityMap.end())
	//		{
	//			return &m_entityMap[entityId];
	//		}
	//		else
	//		{
	//			return nullptr;
	//		}
	//	}

	//	std::vector<Entity> m_entities;
	//	std::unordered_map<Entity::Id, Entity> m_entityMap; 
	//	std::vector<std::unique_ptr<ChunkArray>> m_componentPools; // Should probably use unique ptr, so we dont have destruction and copy construction
	//};
}

