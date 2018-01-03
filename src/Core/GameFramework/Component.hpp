#pragma once

#include <stdint.h>
#include <limits>

namespace Phoenix
{
	typedef uint64_t ComponentMask;

	typedef size_t ComponentTypeId;

	class ComponentHandle
	{
	public:
		explicit ComponentHandle()
			: m_id(invalidValue())
		{}

		explicit ComponentHandle(uint64_t index)
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

		bool operator ==(const ComponentHandle& rhv) const
		{
			return m_id == rhv.m_id;
		}

		bool operator !=(const ComponentHandle& rhv) const
		{
			return m_id != rhv.m_id;
		}
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
		static ComponentTypeId getType()
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

		uint64_t m_usingEntity;
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
}