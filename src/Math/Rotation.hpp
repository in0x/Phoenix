#pragma once

namespace Phoenix::Math
{
	class Matrix3;
	class Vec3;
	class Quaternion;

	Vec3 mat3ToEuler(const Matrix3& mat);
	Vec3 quatToEuler(const Quaternion& quat);
	
	Matrix3 eulerToMat3(const Vec3& vec);
	Matrix3 quatToMat3(const Quaternion& quat);

	Quaternion eulerToQuat(const Vec3& vec);
}