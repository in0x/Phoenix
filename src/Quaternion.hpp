#pragma once

#include "Vec3.hpp"

namespace Phoenix::Math
{	
	class Quaternion
	{
	public:		
		float w;
		Vec3 v;
		
		Quaternion(float w, const Vec3& v);

		static Quaternion fromExpMap(float theta, const Vec3& n);
		static Quaternion fromEulerAngles(const Vec3& angles);

		Matrix4 toMatrix4();

		Quaternion& operator*=(const Quaternion& rhv);
		
		Quaternion& operator*=(float rhv);
		
		float magnitude();

		void conjugateSelf();
		Quaternion conjugate() const;

		void inverseSelf();
		Quaternion inverse() const;

		float dot(const Quaternion& rhv) const;
	};

	Quaternion operator*(Quaternion lhv, float rhv);
	Quaternion operator*(Quaternion lhv, const Quaternion& rhv);
	Quaternion slerp(const Quaternion& a, const Quaternion& b, float t);
}