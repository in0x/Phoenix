#pragma once

#include "Vec3.hpp"

namespace phoenix::math
{	
	class quaternion
	{
	public:
		quaternion(float theta, vec3 n);

		float w;
		vec3 v;

		quaternion operator*=(const quaternion& rhv);
		
		quaternion operator*=(float rhv);

			
		quaternion conjugate();
		void conjugateSelf();

		quaternion inverse();
		void inverseSelf();


	};

	inline quaternion operator*(quaternion lhv, float rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	inline quaternion operator*(quaternion lhv, const quaternion& rhv)
	{
		lhv *= rhv;
		return lhv;
	}
}