#pragma once

#include "CTransform.hpp"
#include <Core/Serialize.hpp>
#include <Core/SerialUtil.hpp>

namespace Phoenix
{
	void CTransform::serial(Archive* ar)
	{
		serialize(ar, m_translation);
		serialize(ar, m_rotation);
		serialize(ar, m_scale);
	}

	void CTransform::setTranslation(const Vec3& t)
	{
		m_translation = t;
		m_bDirty = true;
	}

	void CTransform::setRotation(const Vec3& r)
	{
		m_rotation = r;
		m_bDirty = true;
	}

	void CTransform::setScale(const Vec3& s)
	{
		m_scale = s;
		m_bDirty = true;
	}

	const Vec3& CTransform::getTranslation() const
	{
		return m_translation;
	}

	const Vec3& CTransform::getRotation() const
	{
		return m_rotation;
	}

	const Vec3& CTransform::getScale() const
	{
		return m_scale;
	}

	void CTransform::save(Archive* ar) 
	{
		serial(ar);
	}

	void CTransform::load(Archive* ar, LoadResources* resources) 
	{
		serial(ar);
		m_bDirty = true;
	}
}