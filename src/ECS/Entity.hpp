#pragma once

#include <stdint.h>

namespace Phoenix
{
	class Entity
	{
		virtual uint64_t getComponentMask() const = 0;
		virtual void* getComponent(uint64_t type) = 0;

		template <typename T>
		T* getComponentT()
		{
			return reinterpret_cast<T*>(getComponent(T::s_type)); // Problem with nullptr?
		}
	};
}