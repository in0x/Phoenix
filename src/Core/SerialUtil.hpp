#pragma once

#include <Core/Serialize.hpp>

#include <string>
#include <vector>

namespace Phoenix
{
	class Vec3;
	class Vec4;

	void serialize(Archive& ar, float& f);

	void serialize(Archive& ar, Vec3& vec3);

	void serialize(Archive& ar, Vec4& vec4);

	void serialize(Archive& ar, size_t& sizet);

	void serialize(Archive& ar, uint8_t& u8);

	void serialize(Archive& ar, std::string& string);

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