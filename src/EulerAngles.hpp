#pragma once

#include "Vec3.hpp"

namespace Phoenix::Math
{
	class EulerAngles
	{
	private:
		Vec3 m_angles;

	public:

		EulerAngles(float x, float y, float z);

		float& x();
		float x() const;

		float& y();
		float y() const;

		float& z();
		float z() const;

	};
}