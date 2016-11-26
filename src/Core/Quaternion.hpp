#pragma once

#include "PhiCoreRequired.hpp"
#include <iostream>

namespace Phoenix
{	
	class Vec3;
	class Matrix4;
	class EulerAngles;

	// A unit-length quaternion used to represent rotations.
	class Quaternion
	{
	public: 
		f32 w;
		f32 x;
		f32 y;
		f32 z;
		Quaternion(f32 w, f32 x, f32 y, f32 z);

		static Quaternion fromExpMap(f32 theta, const Vec3& n);
		static Quaternion fromEulerAngles(const Vec3& angles);

		Matrix4 toMatrix4() const;

		Quaternion& operator*=(const Quaternion& rhv);
		Quaternion& operator*=(f32 rhv);
		bool operator==(const Quaternion& rhv) const;
		
		f32 magnitude2();
		f32 magnitude();
		void normalize();

		void conjugateSelf();
		Quaternion conjugate() const;

		void inverseSelf();
		Quaternion inverse() const;

		f32 dot(const Quaternion& rhv) const;

		EulerAngles asEulerAngles() const;

		static Quaternion fromEulerAngles(const EulerAngles& angles);
	};

	Quaternion operator*(Quaternion lhv, f32 rhv);
	Quaternion operator*(Quaternion lhv, const Quaternion& rhv);
	Quaternion slerp(Quaternion a, const Quaternion& b, f32 t);

	inline std::ostream& operator<<(std::ostream& out, const Quaternion& quat)
	{
		out << "( w: " << quat.w << " x: " << quat.x << " y: " << quat.y << " z: " << quat.z << " )";
		return out;
	}
}