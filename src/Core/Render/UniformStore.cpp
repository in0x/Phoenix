#include "UniformStore.hpp"

#include "../Memory/MemUtil.hpp"
#include "../Math/PhiMath.hpp"

#include <assert.h>

namespace Phoenix
{
	UniformStore::UniformStore(size_t count)
		: m_memory(sizeof(Matrix4), count, alignof(Matrix4))
		, m_uniforms(count)
	{
	}

	const UniformInfo& UniformStore::create(size_t id, EUniform::Type type, const char* name, const void* data, size_t size)
	{
		assert(UniformHandle::invalidValue != id);
		assert(id < m_uniforms.size());
		assert(m_uniforms[id].data == nullptr);

		void* dataPtr = m_memory.allocate(size, alignof(Matrix4));
		memcpy(dataPtr, data, size);

		UniformInfo& info = m_uniforms[id];

		info.data = dataPtr;
		info.size = size;
		info.type = type;
		info.nameHash = HashFNV<const char*>()(name);
		
		assert(RenderConstants::c_maxUniformNameLenght >= strlen(name));
		strcpy(info.name, name);

		return info;
	}

	const UniformInfo& UniformStore::get(size_t id)
	{
		assert(UniformHandle::invalidValue != id);
		assert(id < m_uniforms.size());

		UniformInfo& info = m_uniforms[id];
		assert(info.data != nullptr);

		return info;
	}

	void UniformStore::update(size_t id, const void* data, size_t size)
	{
		assert(UniformHandle::invalidValue != id);
		assert(id < m_uniforms.size());

		UniformInfo& info = m_uniforms[id];

		assert(info.data != nullptr);
		memcpy(info.data, data, size);
	}

	void UniformStore::destroy(size_t id)
	{
		assert(UniformHandle::invalidValue != id);
		assert(id < m_uniforms.size());

		UniformInfo& info = m_uniforms[id];
		assert(info.data != nullptr);
		m_memory.free(info.data);
	}
}