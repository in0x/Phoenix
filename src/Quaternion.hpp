#pragma once

#include "Vec3.hpp"

namespace Phoenix::Math
{	
	class Quaternion
	{
	private:
		float m_angle;
		Vec3 m_axis;

	public:		
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
	};

	Quaternion operator*(Quaternion lhv, float rhv);
	Quaternion operator*(Quaternion lhv, const Quaternion& rhv);
	Quaternion slerp(const Quaternion& a, const Quaternion& b, float t);
}