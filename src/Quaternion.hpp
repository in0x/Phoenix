#pragma once

#include "Vec3.hpp"

namespace Phoenix::Math
{	
	class Quaternion
	{
	public:		
		float w;
		Vec3 v;

		Quaternion(float theta, const Vec3& n);

		Quaternion& operator*=(const Quaternion& rhv);
		
		Quaternion& operator*=(float rhv);

		Quaternion conjugate() const;
		void conjugateSelf();

		Quaternion inverse() const;
		void inverseSelf();

		Vec3 transform(const Vec3& point) const;
	};

	Quaternion operator*(Quaternion lhv, float rhv);
	Quaternion operator*(Quaternion lhv, const Quaternion& rhv);
	Quaternion slerp(const Quaternion& a, const Quaternion& b, float t);
}