#pragma once

#include <Core/Serialize.hpp>

#include <string>

namespace Phoenix
{
	static void serialize(Archive& ar, size_t& sizet)
	{
		ar.serialize(&sizet, sizeof(size_t));
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
			size_t len = string.length();
			serialize(ar, len);

			char* buf = new char[len];
	
			memcpy(buf, string.c_str(), len);
			ar.serialize(buf, len);
		
			delete[] buf;
		}
	}
}