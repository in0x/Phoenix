#pragma once

#include <Core/EntityHandle.hpp>
#include <Core/ECType.hpp>

namespace Phoenix
{
	class Archive;
	struct LoadResources;

#define IMPL_EC_TYPE_ID(typeEnum, typeNamePretty) \
	static ECType staticType() \
	{ \
		return typeEnum; \
	} \
	\
	virtual ECType type() const override \
	{ \
		return typeEnum; \
	} \
	\
	virtual const char* typeName() const override \
	{ \
		return typeNamePretty; \
	} \


	class Component
	{
	public:
		Component() : m_owner(0) {}

		virtual ECType type() const = 0;
		virtual const char* typeName() const = 0;

		virtual void save(Archive* ar) = 0;
		virtual void load(Archive* ar, LoadResources* resources) = 0;

		EntityHandle m_owner;
	};
}