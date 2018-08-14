#include "SerialUtil.hpp"

#include <assert.h>

#include <Math/Vec3.hpp>
#include <Math/Vec4.hpp>

namespace Phoenix
{
	void serialize(Archive* ar, float& f)
	{
		ar->serialize(&f, sizeof(float));
	}

	void serialize(Archive* ar, Vec3& vec3)
	{
		ar->serialize(&vec3, sizeof(vec3));
	}

	void serialize(Archive* ar, Vec4& vec4)
	{
		ar->serialize(&vec4, sizeof(vec4));
	}

	void serialize(Archive* ar, size_t& sizet)
	{
		ar->serialize(&sizet, sizeof(size_t));
	}

	void serialize(Archive* ar, uint8_t& u8)
	{
		ar->serialize(&u8, sizeof(uint8_t));
	}

	void serialize(Archive* ar, int32_t& i32)
	{
		ar->serialize(&i32, sizeof(int32_t));
	}

	void serialize(Archive* ar, std::string& string)
	{
		if (ar->isReading())
		{
			size_t len = 0;
			serialize(ar, len);

			char* buf = new char[len];
			ar->serialize(buf, len);

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
			ar->serialize(buf, len);

			delete[] buf;
		}
	}

}