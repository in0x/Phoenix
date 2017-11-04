#pragma once

namespace Phoenix
{	
	class Vec3;
	class Matrix4;
	class EulerAngles;

	// A unit-length quaternion used to represent rotations.
	class Quaternion
	{
	public: 
		float w;
		float x;
		float y;
		float z;
		Quaternion(float w, float x, float y, float z);

		static Quaternion fromExpMap(float theta, const Vec3& n);
		//static Quaternion fromEulerAngles(const Vec3& angles);

		Matrix4 toMatrix4() const;

		Quaternion& operator*=(const Quaternion& rhv);
		Quaternion& operator*=(float rhv);
		bool operator==(const Quaternion& rhv) const;
		
		float magnitude2();
		float magnitude();
		void normalize();

		void conjugateSelf();
		Quaternion conjugate() const;

		void inverseSelf();
		Quaternion inverse() const;

		float dot(const Quaternion& rhv) const;

		EulerAngles asEulerAngles() const;

		static Quaternion fromEulerAngles(const EulerAngles& angles);
	};

	Quaternion operator*(Quaternion lhv, float rhv);
	Quaternion operator*(Quaternion lhv, const Quaternion& rhv);
	Quaternion slerp(Quaternion a, const Quaternion& b, float t);
}