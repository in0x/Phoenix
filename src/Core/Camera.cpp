#include "Camera.hpp"

namespace Phoenix
{
	Camera::Camera()
		: m_position(0.f, 0.f, 0.f)
		, m_xAxis(1.0f, 0.0f, 0.0f)
		, m_zAxis(0.0f, 0.0f, 1.0f)
		, m_yAxis(0.0f, 1.0f, 0.0f)
	{}

	void Camera::moveRight(float delta)
	{
		m_position += m_xAxis * delta;
	}

	void Camera::moveForward(float delta)
	{
		m_position += m_zAxis * delta;
	}

	void Camera::pitch(float angle)
	{
		Matrix4 r = Matrix4::rotation(angle, m_xAxis);
		m_yAxis = r * m_yAxis;
		m_zAxis = r * m_zAxis;
	}

	void Camera::yaw(float angle)
	{
		Matrix4 r = Matrix4::rotation(angle, Vec3(0.0f, 1.0f, 0.0f));
		m_xAxis = r * m_xAxis;
		m_zAxis = r * m_zAxis;
	}

	Matrix4 Camera::getUpdatedViewMatrix()
	{
		m_zAxis = m_zAxis.normalize();
		m_yAxis = m_zAxis.cross(m_xAxis).normalized();
		m_xAxis = m_yAxis.cross(m_zAxis).normalized();

		return Matrix4{
			m_xAxis.x, m_xAxis.y, m_xAxis.z, -(m_xAxis.dot(m_position)),
			m_yAxis.x, m_yAxis.y, m_yAxis.z, -(m_yAxis.dot(m_position)),
			m_zAxis.x, m_zAxis.y, m_zAxis.z, -(m_zAxis.dot(m_position)),
			0,		 0,		  0,		1
		};
	}

}