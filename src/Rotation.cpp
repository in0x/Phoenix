#include "Rotation.hpp"
#include "Vec3.hpp"
#include "Matrix3.hpp"
#include "Quaternion.hpp"
#include "PhiMath.hpp"

namespace Phoenix::Math
{
	// http://www.flipcode.com/documents/matrfaq.html#Q36
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

	// http://www.flipcode.com/documents/matrfaq.html#Q37
	Vec3 mat3ToEuler(const Matrix3& mat)
	{
		float x, y, z;

		y = -std::asin(mat(0, 2));
		auto C = std::cos(y);

		if (std::abs(C) > 0.005f) 
		{
			auto trX = mat(2,2) / C;         
			auto trY = -mat(1,2) / C;

			x = std::atan2(trY, trX);
			
			trX = mat(0,0) / C;            
			trY = -mat(0,1) / C;

			z = std::atan2(trY, trX);
		}
		else // Gimball Lock
		{
			x = 0;

			auto trX = mat(1,1);                
			auto trY = mat(1,0);

			z = std::atan2(trY, trX);
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