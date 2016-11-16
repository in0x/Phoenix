#pragma once

namespace Phoenix::Math
{
	class Matrix3;
	class Vec3;
	class Quaternion;

	Matrix3 eulerToMat3(const Vec3& vec);
	Vec3 mat3ToEuler(const Matrix3& mat);

	Vec3 quatToEuler(const Quaternion& quat);
	Quaternion eulerToQuat(const Vec3& vec);

	Matrix3 quatToMat3(const Quaternion& quat);

	class Rotation
	{
	private:
		Rotation();
	public:

		static Rotation fromEulerAngles(float xAngle, float yAngle, float zAngle);
		static Rotation fromEulerAngles(const Vec3& angles);

	};
}