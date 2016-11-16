#include "Rotation.hpp"
#include "Vec3.hpp"
#include "Matrix3.hpp"
#include "Quaternion.hpp"
#include "Common.hpp"

namespace Phoenix::Math
{
	// Basically, we directly concatenate the 3 matrices for
	// yaw, pitch and roll, saving us a lot of superflous operations
	// that 3 seperate matrices would entail.
	Matrix3 eulerToMat3(const Vec3& vec)
	{
		auto x = radians(vec.x);
		auto y = radians(vec.y);
		auto z = radians(vec.z);

		auto A = std::cos(x); 
		auto B = std::sin(x);  
		auto C = std::cos(y); 
		auto D = std::sin(y); 
		auto E = std::cos(z); 
		auto F = std::sin(z); 

		return Matrix3
		{
			C*E,         -C*F,         -D,
		   -B*D*E + A*F,  B*D*F + A*E, -B*C,
			A*D*E + B*F, -A*D*F + B*E,  A*C
		};
	}

	Vec3 mat3ToEuler(const Matrix3& mat)
	{
		float x, y, z;

		y = -std::asin(mat(0, 2));
		auto C = std::cos(y);

		if (std::abs(C) > 0.005f) // Account for floating point inaccuracy
		{
			auto A = mat(2,2) / C;         
			auto B = -mat(1,2) / C;

			// tanx = sinx / cosx
			x = std::atan2(B, A);
	
			auto E = mat(0,0) / C;            
			auto F = -mat(0,1) / C;

			z = std::atan2(F, E);
		}
		else // Gimball Lock
		{
			x = 0;
			
			// D is 1
			auto BEAF = mat(1,1);                
			auto AEBF = mat(1,0);

			z = std::atan2(AEBF, BEAF);
		}

		return Vec3{ degrees(x), degrees(y), degrees(z) };
	}

	Vec3 quatToEuler(const Quaternion& quat)
	{
		float heading, pitch, bank;

		float sqw = quat.w*quat.w;
		float sqx = quat.x*quat.x;
		float sqy = quat.y*quat.y;
		float sqz = quat.z*quat.z;
		float unit = sqx + sqy + sqz + sqw; // allows this to work on non-normalized quats.
		float test = quat.x*quat.y + quat.z*quat.w;

		if (test > 0.499f * unit) // Gimbal lock looking up.
		{
			heading = 2.f * atan2(quat.x, quat.w);
			pitch = pi() / 2.f;
			bank = 0.f;
		}
		else if (test < -0.499f * unit) // Gimbal lock looking down.
		{ 
			heading = -2.f * atan2(quat.x, quat.w);
			pitch = -pi() / 2.f;
			bank = 0.f;
		}
		else
		{
			heading = atan2(2.f * quat.y*quat.w - 2.f * quat.x*quat.z, sqx - sqy - sqz + sqw);
			pitch = asin(2.f * test / unit);
			bank = atan2(2.f * quat.x*quat.w - 2.f * quat.y*quat.z, -sqx + sqy - sqz + sqw);
		}

		return Vec3{ degrees(heading), degrees(pitch), degrees(bank) };
	}

	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/index.htm
	Quaternion eulerToQuat(const Vec3& vec) 
	{
		auto x = radians(vec.x);
		auto y = radians(vec.y);
		auto z = radians(vec.z);

		float c1 = std::cos(y / 2);
		float s1 = std::sin(y / 2);
		float c2 = std::cos(x / 2);
		float s2 = std::sin(x / 2);
		float c3 = std::cos(z / 2);
		float s3 = std::sin(z / 2);
		float c1c2 = c1*c2;
		float s1s2 = s1*s2;

		return Quaternion{ c1c2*c3 - s1s2*s3 , c1c2*s3 + s1s2*c3, s1*c2*c3 + c1*s2*s3, c1*s2*c3 - s1*c2*s3 };
	}

	Matrix3 quatToMat3(const Quaternion& quat)
	{
		return Matrix3{
			1 - 2*quat.y*quat.y - 2*quat.z*quat.z, 2*quat.x*quat.y + 2*quat.w*quat.z, 2*quat.x*quat.z - 2*quat.w*quat.y,
			2*quat.x*quat.y - 2*quat.w*quat.z, 1 - 2*quat.x*quat.x - 2*quat.z*quat.z, 2*quat.y*quat.z + 2*quat.w*quat.x,
			2*quat.x*quat.z + 2*quat.w*quat.y, 2*quat.y*quat.z - 2*quat.w*quat.x, 1 - 2*quat.x*quat.x - 2*quat.y*quat.y
		};
	}
}