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
	
	class ComponentHandle
	{
	public:
		ComponentHandle()
			: m_id(invalidValue())
		{}

		ComponentHandle(uint64_t index)
			: m_id(index)
		{}

		uint64_t m_id;

		static constexpr uint64_t maxValue()
		{
			return std::numeric_limits<uint64_t>::max() - 1;
		}

		static constexpr uint64_t invalidValue()
		{
			return maxValue() + 1;
		}

		void invalidate()
		{
			m_id = invalidValue();
		}

		bool isValid() const
		{
			return m_id != invalidValue();
		}
	};

	struct World;

	struct Entity
	{
		typedef uint64_t Id;

		Entity(Id id, World* world)
			: m_id(id)
			, m_worldIn(world)
			, m_componentMask(0)
		{}

		template <typename Comp>
		void registerComponent(ComponentHandle handle)
		{
			assert(m_componentIds.find(Comp::getType()) == m_componentIds.end());
			
			m_componentIds.emplace(Comp::getType(), handle);
			m_componentMask |= Comp::getMask();
		}

		template <typename Comp>
		void unregisterComponent()
		{
			assert(m_componentIds.find(Comp::getType()) != m_componentIds.end());

			m_componentIds[Comp::getType()].invalidate();
			m_componentMask &= ~(1i64 << Comp::getMask());
		}

		template <typename Comp, typename ...CtorArgs>
		Comp* addComponent(CtorArgs... ctorArgs)
		{
			return m_worldIn->addComponent<Comp, CtorArgs...>(m_id, std::forward(ctorArgs)...);
		}

		template <typename Comp>
		bool hasComponent()
		{
			return (m_componentMask & Comp::getMask()) == Comp::getMask();
		}

		template <typename Comp>
		Comp* getComponent()
		{
			if (hasComponent<Comp>())
			{
				return m_worldIn->getComponent<Comp>(m_id, m_componentIds[Comp::getType()]);
			}
			else
			{
				return nullptr;
			}
		}

		template <typename Comp>
		void removeComponent()
		{
			m_worldIn->removeComponent<Comp>(m_id);
		}

		void reset()
		{
			m_componentMask = 0;
			m_componentIds.clear();
		}

		Id m_id;
		World* m_worldIn;
		ComponentMask m_componentMask;
		std::unordered_map<ComponentTypeId, ComponentHandle> m_componentIds;
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

	template <typename Derived>
	struct Component : public ComponentBase 
	{
		static ComponentTypeId getType() // Type can be used to lookup eg object pools in world
		{
			static ComponentTypeId type = ComponentBase::s_componentTypeCounter++;
			assert(type < ComponentBase::MaxComponentTypes);
			return type;
		}

		static ComponentMask getMask()
		{
			static ComponentMask mask = 1i64 << getType();
			return mask;
		}

		Entity::Id m_usingEntity;
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
			, m_elemsPerChunk(chunkSizeBytes / elemSizeBytes)
		{
		}

		ChunkArrayBase(size_t elemSizeBytes, size_t chunkSizeBytes, size_t initialCapacity)
			: m_sizePerAllocBytes(elemSizeBytes)
			, m_chunkSizeBytes(chunkSizeBytes)
			, m_lastAllocIdx(0)
			, m_elemsPerChunk(chunkSizeBytes / elemSizeBytes)
		{
			expandTo(initialCapacity);
		}

		virtual ~ChunkArrayBase()
		{
			for (char* chunk : m_chunks)
			{
				operator delete(chunk);
			}
		}

		size_t capacity() const
		{
			size_t c = m_elemsPerChunk * m_chunks.size();
			return c;
		}

		size_t size() const
		{
			size_t s = (m_lastAllocIdx / m_elemsPerChunk) * m_elemsPerChunk + (m_lastAllocIdx % m_elemsPerChunk);
			return s;
		}

		void* at(size_t idx)
		{
			assert(idx <= m_lastAllocIdx);
			return m_chunks[idx / m_elemsPerChunk] + (idx % m_elemsPerChunk) * m_sizePerAllocBytes;
		}

		void* alloc()
		{
			size_t next = m_lastAllocIdx++;
			expandTo(next);
			return at(next);
		}

	private:
		void expandTo(size_t numElements)
		{
			while (capacity() <= numElements)
			{
				m_chunks.push_back(static_cast<char*>(operator new(m_chunkSizeBytes)));
			}
		}

		std::vector<char*> m_chunks;
		size_t m_sizePerAllocBytes;
		size_t m_chunkSizeBytes;
		size_t m_lastAllocIdx;
		size_t m_elemsPerChunk;
	};


	template<typename T>
	class ChunkArray : public ChunkArrayBase
	{
	public:
		class ChunkArray(size_t elementsPerChunk) 
			: ChunkArrayBase(sizeof(T), elementsPerChunk * sizeof(T))
		{}

		class ChunkArray(size_t elementsPerChunk, size_t initialCapacity)
			: ChunkArrayBase(sizeof(T), elementsPerChunk * sizeof(T), initialCapacity)
		{}	

		~ChunkArray()
		{
			size_t numAlloced = size();
			for (size_t i = 0; i < numAlloced; ++i)
			{
				operator[](i)->~T();
			}
		}

		T* operator[](size_t idx)
		{
			return static_cast<T*>(at(idx));
		}

		template <typename ...CtorArgs>
		T* add(CtorArgs... ctorArgs)
		{
			return new (alloc()) T(ctorArgs...);
		}
	}; 

	struct World
	{
		World()
			: m_entityIdCounter(0)
		{}

		template <typename Comp>
		void registerComponentType()
		{
			static_assert(is_derived_component<Comp>::value, "Registered component types need to inherit from Component");

			ComponentTypeId type = Comp::getType();

			if (m_componentPools.size() <= type)
			{
				auto pool = std::make_unique<ChunkArray<Comp>>(128);
				m_componentPools.push_back(std::move(pool));
			}
		}

		template <typename Comp, typename ...CtorArgs>
		Comp* addComponent(Entity::Id entityId, CtorArgs... ctorArgs)
		{
			Entity* entity = getEntity(entityId);
			assert(entity != nullptr);
			assert(!entity->hasComponent<Comp>());

			ChunkArray<Comp>* pool = getComponentPool<Comp>();
			Comp* comp = pool->add(std::forward(ctorArgs)...);
			entity->registerComponent<Comp>(pool->size());
			return comp;
		}

		template <typename Comp>
		Comp* getComponent(Entity::Id entityId, ComponentHandle compId)
		{
			Entity* entity = getEntity(entityId);
			assert(entity != nullptr);
			
			ChunkArray<Comp>* pool = getComponentPool<Comp>();
			Comp* comp = pool[compId];
			return comp;
		}

		template <typename Comp>
		void removeComponent(Entity::Id entityId)
		{
			Entity* entity = getEntity(entityId);
			entity->unregisterComponent<Comp>();
			// For now, we just leave the component alive. Linearly iterating components while acessing multiple
			// in one system is pretty difficult, so we're just gonna tick entities with the right mask for now.
			// This approach can be probably be sped up by caching the correct entity sets and patching those when
			// the world is changed.
		}

		template <typename Comp>
		ChunkArray<Comp>* getComponentPool()
		{
			ComponentTypeId type = Comp::getType();
			assert(type < m_componentPools.size());
			return static_cast<ChunkArray<Comp>*>(m_componentPools[type].get());
		}

		Entity::Id createEntity()
		{
			// TODO: Check if we can reuse any entities
			
			Entity::Id id = m_entityIdCounter++;
			assert(id < std::numeric_limits<Entity::Id>::max());
			
			m_entities.emplace_back(id, this);
			m_entityMap.emplace(id, &m_entities.back());
			
			return id;
		}

		void destroyEntity(Entity::Id entityId)
		{
			Entity* entity = getEntity(entityId);
			entity->reset();
			m_entityMap[entityId] = nullptr;
			// TODO: return the entity to reusable list
		}

		Entity* getEntity(Entity::Id entityId)
		{
			assert(entityId < m_entityIdCounter);
			assert(m_entityMap.find(entityId) != m_entityMap.end());
			return m_entityMap[entityId];
		}

		std::vector<Entity> m_entities;
		uint64_t m_entityIdCounter;
		std::unordered_map<Entity::Id, Entity*> m_entityMap; 
		std::vector<std::unique_ptr<ChunkArrayBase>> m_componentPools;
	};

	template <typename ...Components>
	class WorldIterator
	{
		// TODO Build a combined mask from the types of Components
		// TODO Iterate each entity
		// TODO Check if the entity statisfies the mask
		// If yes, stop, else iterate again
	};
}

