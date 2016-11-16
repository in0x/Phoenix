#include "Rotation.hpp"
#include "Vec3.hpp"
#include "Matrix3.hpp"
#include "Quaternion.hpp"
#include "PhiMath.hpp"

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
		return {};
	}

	Quaternion eulerToQuat(const Vec3& vec)
	{
		return{1,1,1,1};
	}

	Matrix3 quatToMat3(const Quaternion& quat)
	{
		return{};
	}
}