#pragma once

#include <Core/Serialize.hpp>

#include <string>
#include <vector>
#include <assert.h>

namespace Phoenix
{
	static void serialize(Archive& ar, size_t& sizet)
	{
		ar.serialize(&sizet, sizeof(size_t));
	}

	static void serialize(Archive& ar, uint8_t& sizet)
	{
		ar.serialize(&sizet, sizeof(uint8_t));
	}

	static void serialize(Archive& ar, std::string& string)
	{
		if (ar.isReading())
		{
			size_t len = 0;
			serialize(ar, len);

			char* buf = new char[len];
			ar.serialize(buf, len);

			string = std::string(buf);	

			delete[] buf;
		}
		else
		{
			assert(!string.empty());

			size_t len = string.size() + 1;
			serialize(ar, len);

			char* buf = new char[len];
	
			strcpy(buf, string.c_str());
			ar.serialize(buf, len);
		
			delete[] buf;
		}
	}

	template <class T>
	static void serialize(Archive& ar, std::vector<T>& vector)
	{
		static_assert(std::is_standard_layout<T>::value, "T should be standard layout.");

		if (ar.isWriting())
		{
			size_t size = vector.size();
			serialize(ar, size); 
			ar.serialize(vector.data(), sizeof(T) * vector.size());
		}
		else
		{
			size_t size = 0;
			serialize(ar, size);

			vector.resize(size);
			ar.serialize(vector.data(), sizeof(T) * size);
		}
	}
}