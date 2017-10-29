#pragma once

#include <stdint.h>
#include <string.h>

namespace Phoenix
{
	typedef uint64_t FNVHash;

	inline FNVHash hashBytes(const void* data, size_t size, uint64_t offset = 14695981039346656037, uint64_t prime = 1099511628211)
	{
		const char* ptr = static_cast<const char*>(data);
		FNVHash hash = offset;

		for (size_t i = 0; i < size; ++i)
		{
			FNVHash next = static_cast<FNVHash>(ptr[i]);
			hash = (hash ^ next) * prime;
		}

		return hash;
	}

	template<uint64_t offset = 14695981039346656037, uint64_t prime = 1099511628211>
	struct HashFNVIterative
	{
		FNVHash m_hash;

	public:
		HashFNVIterative()
			: m_hash(offset)
		{}

		void add(const void* data, size_t size)
		{
			const char* ptr = static_cast<const char*>(data);

			for (size_t i = 0; i < size; ++i)
			{
				FNVHash next = static_cast<FNVHash>(ptr[i]);
				m_hash = (m_hash ^ next) * prime;
			}
		}

		FNVHash operator()()
		{
			return m_hash;
		}
	};

	template <class T>
	struct HashFNV
	{
		FNVHash operator()(const T& obj)
		{
			auto hash = std::hash<T>();
			return static_cast<uint64_t>(hash(obj));
		}
	};

	template <>
	struct HashFNV<const char*>
	{
		FNVHash operator()(const char* str)
		{
			return hashBytes(str, strlen(str));
		}
	};

}