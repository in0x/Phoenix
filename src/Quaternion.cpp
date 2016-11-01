#pragma once

#include <cmath>

#include "Quaternion.hpp"              
#include "Vec3.hpp"
#include "Matrix4.hpp"

namespace Phoenix::Math
{
	Quaternion::Quaternion(float w, const Vec3& v)
		: m_angle(w)
		, m_axis(v)
	{}

	Quaternion Quaternion::fromExpMap(float theta, const Vec3& n)
	{
		return Quaternion{ std::cos(theta / 2.f), Vec3{std::sin(n.x / 2.f), std::sin(n.y / 2.f) , std::sin(n.z / 2.f) } };
	}

	Quaternion Quaternion::fromEulerAngles(const Vec3& angles)
	{
		return{ 0, Vec3{0,0,0} };
	}

	Matrix4 Quaternion::toMatrix4()
	{
		return{};
	}

	Quaternion& Quaternion::operator*=(const Quaternion& rhv)
	{
		return *this;
	}

	Quaternion& Quaternion::operator*=(float rhv)
	{
		m_angle *= rhv;
		m_axis *= rhv;
		return *this;
	}

	float Quaternion::magnitude()
	{
		return std::sqrtf(m_angle * m_angle + m_axis.length());
	}

	void Quaternion::conjugateSelf()
	{
		m_axis = -m_axis;
	}

	Quaternion Quaternion::conjugate() const
	{
		Quaternion conj = *this;
		conj.conjugateSelf();
		return conj;
	}

	void Quaternion::inverseSelf()
	{
		conjugateSelf();
		auto mag = magnitude();
		m_angle /= mag;
		m_axis /= mag;
	}

	Quaternion Quaternion::inverse() const
	{
		Quaternion inverted = *this;
		inverted.inverseSelf();
		return inverted;
	}

	Quaternion operator*(Quaternion lhv, float rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	Quaternion operator*(Quaternion lhv, const Quaternion& rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	Quaternion slerp(const Quaternion& a, const Quaternion& b, float t)
	{
		return{ 0, Vec3{0,0,0} };
	}
}