#pragma once

#include <Core/Component.hpp>
#include <Math/Vec3.hpp>

namespace Phoenix
{
	class CTransform : public Component
	{
		Vec3 m_translation;
		Vec3 m_rotation;
		Vec3 m_scale;

		void serial(Archive* ar);

	public:
		void setTranslation(const Vec3& t);
		void setRotation(const Vec3& r);
		void setScale(const Vec3& s);


		const Vec3& getTranslation() const;
		const Vec3& getRotation() const;
		const Vec3& getScale() const;

		virtual void save(Archive* ar) override;
		virtual void load(Archive* ar, LoadResources* resources) override;

		Matrix4* m_transform;
		bool m_bDirty;

		IMPL_EC_TYPE_ID(ECType::CT_Transform, "Transform");
	};
}