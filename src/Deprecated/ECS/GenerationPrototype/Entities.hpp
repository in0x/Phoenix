#pragma once
#include <ECS/Entity.hpp>

// Entities.hpp: Generated 2018-06-17 20:36:20.679483 UTC
// A set of all possible entities generated from the existing set of components.

#include <ECS/Components/CDirectionalLight.hpp>
#include <ECS/Components/CPointLight.hpp>
#include <ECS/Components/CStaticMesh.hpp>
#include <ECS/Components/CTransform.hpp>

struct GeneratedEntity0 : public Entity
{
	CDirectionalLight m_CDirectionalLight

	virtual uint64_t getComponentMask() const override
	{
		return 0 | CDirectionalLight::s_type;
	}

	virtual void* getComponent(uint64_t type) override
	{
		switch (type)
		{
		case CDirectionalLight::s_type:
		{
			return &m_CDirectionalLight;
		}
	}
};

struct GeneratedEntity1 : public Entity
{
	CPointLight m_CPointLight

	virtual uint64_t getComponentMask() const override
	{
		return 0 | CPointLight::s_type;
	}

	virtual void* getComponent(uint64_t type) override
	{
		switch (type)
		{
		case CPointLight::s_type:
		{
			return &m_CPointLight;
		}
	}
};

struct GeneratedEntity2 : public Entity
{
	CStaticMesh m_CStaticMesh

	virtual uint64_t getComponentMask() const override
	{
		return 0 | CStaticMesh::s_type;
	}

	virtual void* getComponent(uint64_t type) override
	{
		switch (type)
		{
		case CStaticMesh::s_type:
		{
			return &m_CStaticMesh;
		}
	}
};

struct GeneratedEntity3 : public Entity
{
	CTransform m_CTransform

	virtual uint64_t getComponentMask() const override
	{
		return 0 | CTransform::s_type;
	}

	virtual void* getComponent(uint64_t type) override
	{
		switch (type)
		{
		case CTransform::s_type:
		{
			return &m_CTransform;
		}
	}
};

struct GeneratedEntity4 : public Entity
{
	CDirectionalLight m_CDirectionalLight
	CPointLight m_CPointLight

	virtual uint64_t getComponentMask() const override
	{
		return 0 | CDirectionalLight::s_type | CPointLight::s_type;
	}

	virtual void* getComponent(uint64_t type) override
	{
		switch (type)
		{
		case CDirectionalLight::s_type:
		{
			return &m_CDirectionalLight;
		}
		case CPointLight::s_type:
		{
			return &m_CPointLight;
		}
	}
};

struct GeneratedEntity5 : public Entity
{
	CDirectionalLight m_CDirectionalLight
	CStaticMesh m_CStaticMesh

	virtual uint64_t getComponentMask() const override
	{
		return 0 | CDirectionalLight::s_type | CStaticMesh::s_type;
	}

	virtual void* getComponent(uint64_t type) override
	{
		switch (type)
		{
		case CDirectionalLight::s_type:
		{
			return &m_CDirectionalLight;
		}
		case CStaticMesh::s_type:
		{
			return &m_CStaticMesh;
		}
	}
};

struct GeneratedEntity6 : public Entity
{
	CDirectionalLight m_CDirectionalLight
	CTransform m_CTransform

	virtual uint64_t getComponentMask() const override
	{
		return 0 | CDirectionalLight::s_type | CTransform::s_type;
	}

	virtual void* getComponent(uint64_t type) override
	{
		switch (type)
		{
		case CDirectionalLight::s_type:
		{
			return &m_CDirectionalLight;
		}
		case CTransform::s_type:
		{
			return &m_CTransform;
		}
	}
};

struct GeneratedEntity7 : public Entity
{
	CPointLight m_CPointLight
	CStaticMesh m_CStaticMesh

	virtual uint64_t getComponentMask() const override
	{
		return 0 | CPointLight::s_type | CStaticMesh::s_type;
	}

	virtual void* getComponent(uint64_t type) override
	{
		switch (type)
		{
		case CPointLight::s_type:
		{
			return &m_CPointLight;
		}
		case CStaticMesh::s_type:
		{
			return &m_CStaticMesh;
		}
	}
};

struct GeneratedEntity8 : public Entity
{
	CPointLight m_CPointLight
	CTransform m_CTransform

	virtual uint64_t getComponentMask() const override
	{
		return 0 | CPointLight::s_type | CTransform::s_type;
	}

	virtual void* getComponent(uint64_t type) override
	{
		switch (type)
		{
		case CPointLight::s_type:
		{
			return &m_CPointLight;
		}
		case CTransform::s_type:
		{
			return &m_CTransform;
		}
	}
};

struct GeneratedEntity9 : public Entity
{
	CStaticMesh m_CStaticMesh
	CTransform m_CTransform

	virtual uint64_t getComponentMask() const override
	{
		return 0 | CStaticMesh::s_type | CTransform::s_type;
	}

	virtual void* getComponent(uint64_t type) override
	{
		switch (type)
		{
		case CStaticMesh::s_type:
		{
			return &m_CStaticMesh;
		}
		case CTransform::s_type:
		{
			return &m_CTransform;
		}
	}
};

struct GeneratedEntity10 : public Entity
{
	CDirectionalLight m_CDirectionalLight
	CPointLight m_CPointLight
	CStaticMesh m_CStaticMesh

	virtual uint64_t getComponentMask() const override
	{
		return 0 | CDirectionalLight::s_type | CPointLight::s_type | CStaticMesh::s_type;
	}

	virtual void* getComponent(uint64_t type) override
	{
		switch (type)
		{
		case CDirectionalLight::s_type:
		{
			return &m_CDirectionalLight;
		}
		case CPointLight::s_type:
		{
			return &m_CPointLight;
		}
		case CStaticMesh::s_type:
		{
			return &m_CStaticMesh;
		}
	}
};

struct GeneratedEntity11 : public Entity
{
	CDirectionalLight m_CDirectionalLight
	CPointLight m_CPointLight
	CTransform m_CTransform

	virtual uint64_t getComponentMask() const override
	{
		return 0 | CDirectionalLight::s_type | CPointLight::s_type | CTransform::s_type;
	}

	virtual void* getComponent(uint64_t type) override
	{
		switch (type)
		{
		case CDirectionalLight::s_type:
		{
			return &m_CDirectionalLight;
		}
		case CPointLight::s_type:
		{
			return &m_CPointLight;
		}
		case CTransform::s_type:
		{
			return &m_CTransform;
		}
	}
};

struct GeneratedEntity12 : public Entity
{
	CDirectionalLight m_CDirectionalLight
	CStaticMesh m_CStaticMesh
	CTransform m_CTransform

	virtual uint64_t getComponentMask() const override
	{
		return 0 | CDirectionalLight::s_type | CStaticMesh::s_type | CTransform::s_type;
	}

	virtual void* getComponent(uint64_t type) override
	{
		switch (type)
		{
		case CDirectionalLight::s_type:
		{
			return &m_CDirectionalLight;
		}
		case CStaticMesh::s_type:
		{
			return &m_CStaticMesh;
		}
		case CTransform::s_type:
		{
			return &m_CTransform;
		}
	}
};

struct GeneratedEntity13 : public Entity
{
	CPointLight m_CPointLight
	CStaticMesh m_CStaticMesh
	CTransform m_CTransform

	virtual uint64_t getComponentMask() const override
	{
		return 0 | CPointLight::s_type | CStaticMesh::s_type | CTransform::s_type;
	}

	virtual void* getComponent(uint64_t type) override
	{
		switch (type)
		{
		case CPointLight::s_type:
		{
			return &m_CPointLight;
		}
		case CStaticMesh::s_type:
		{
			return &m_CStaticMesh;
		}
		case CTransform::s_type:
		{
			return &m_CTransform;
		}
	}
};

struct GeneratedEntity14 : public Entity
{
	CDirectionalLight m_CDirectionalLight
	CPointLight m_CPointLight
	CStaticMesh m_CStaticMesh
	CTransform m_CTransform

	virtual uint64_t getComponentMask() const override
	{
		return 0 | CDirectionalLight::s_type | CPointLight::s_type | CStaticMesh::s_type | CTransform::s_type;
	}

	virtual void* getComponent(uint64_t type) override
	{
		switch (type)
		{
		case CDirectionalLight::s_type:
		{
			return &m_CDirectionalLight;
		}
		case CPointLight::s_type:
		{
			return &m_CPointLight;
		}
		case CStaticMesh::s_type:
		{
			return &m_CStaticMesh;
		}
		case CTransform::s_type:
		{
			return &m_CTransform;
		}
	}
};

