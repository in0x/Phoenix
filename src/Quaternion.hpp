#pragma once

#include <iostream>

namespace Phoenix::Math
{	
	class Vec3;
	class Matrix4;

	// A unit-length quaternion used to represent rotations.
	class Quaternion
	{
	private:
		
		void normalize();

	public: 
		float w;
		float x;
		float y;
		float z;
		Quaternion(float w, float x, float y, float z);

		static Quaternion fromExpMap(float theta, const Vec3& n);
		static Quaternion fromEulerAngles(const Vec3& angles);

		Matrix4 toMatrix4() const;

		Quaternion& operator*=(const Quaternion& rhv);
		
		Quaternion& operator*=(float rhv);
		
		float magnitude2();
		float magnitude();

		void conjugateSelf();
		Quaternion conjugate() const;

		void inverseSelf();
		Quaternion inverse() const;

		float dot(const Quaternion& rhv) const;
	};

	Quaternion operator*(Quaternion lhv, float rhv);
	Quaternion operator*(Quaternion lhv, const Quaternion& rhv);
	Quaternion slerp(Quaternion a, const Quaternion& b, float t);

	inline std::ostream& operator<<(std::ostream& out, const Quaternion& quat)
	{
		out << "( w: " << quat.w << " x: " << quat.x << " y: " << quat.y << " z: " << quat.z << " )";
		return out;
	}
}