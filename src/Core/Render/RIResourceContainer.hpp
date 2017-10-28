#pragma once

namespace Phoenix
{
	template<class Resource, class Handle, size_t numMaxResources>
	class RIResourceContainer
	{
	public:
		static_assert(numMaxResources <= Handle::maxValue(), "numMaxResource must not exceed maxValue of Handle type");

		RIResourceContainer()
		{
			m_head = &m_resources[0];

			for (size_t i = 1; i < numMaxResources; ++i)
			{
				m_resources[i - 1].m_next = &m_resources[i];
			}

			m_resources[numMaxResources - 1].m_next = nullptr;
		}

		~RIResourceContainer()
		{
			for (size_t i = 0; i < numMaxResources; ++i)
			{
				delete m_resources[i].m_pToResource;
			}
		}

		Resource* getResource(Handle handle)
		{
			return m_resources[handle.m_idx].m_pToResource;
		}

		Handle allocateResource()
		{
			Handle handle;
			handle.invalidate();

			if (nullptr == m_head)
			{
				return handle;
			}

			ResourceListSlot* item = m_head;
			m_head = m_head->m_next;

			item->m_pToResource = new Resource;

			handle.m_idx = reinterpret_cast<char*>(item) - reinterpret_cast<char*>(&m_resources[0]);
			return handle;
		}

		void destroyResource(Handle handle)
		{
			ResourceListSlot* item = &m_resources[handle.m_idx];
			delete item->m_pToResource;
			item->m_pToResource = nullptr;

			ResourceListSlot* temp = m_head;
			m_head = item;
			item->m_next = temp;
		}

		size_t getMaxResourceCount() const
		{
			return numMaxResources;
		}

	private:
		struct ResourceListSlot
		{
			ResourceListSlot()
				: m_pToResource(nullptr)
				, m_next(nullptr)
			{}

			Resource* m_pToResource;
			ResourceListSlot* m_next;
		};

		ResourceListSlot* m_head;
		ResourceListSlot m_resources[numMaxResources];
	};
}