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

	const UniformInfo& UniformStore::create(UniformHandle handle, Uniform::Type type, const char* name, const void* data, size_t size)
	{
		assert(handle.isValid());
		assert(handle.idx < m_uniforms.size());
		assert(m_uniforms[handle.idx].data == nullptr);

		void* dataPtr = m_memory.allocate(size, alignof(Matrix4));
		memcpy(dataPtr, data, size);

		UniformInfo& info = m_uniforms[handle.idx];

		info.data = dataPtr;
		info.size = size;
		info.type = type;
		info.nameHash = HashFNV<const char*>()(name);

		//assert(strlen(name) < RenderConstants::c_maxUniformNameLenght);
		//strcpy(info.name, name);

		return info;
	}

	const UniformInfo& UniformStore::get(UniformHandle handle)
	{
		assert(handle.isValid());
		assert(handle.idx < m_uniforms.size());

		UniformInfo& info = m_uniforms[handle.idx];
		assert(info.data != nullptr);

		return info;
	}

	void UniformStore::update(UniformHandle handle, const void* data, size_t size)
	{
		assert(handle.isValid());
		assert(handle.idx < m_uniforms.size());

		UniformInfo& info = m_uniforms[handle.idx];

		assert(info.data != nullptr);
		memcpy(info.data, data, size);
	}

	void UniformStore::destroy(UniformHandle handle)
	{
		assert(handle.isValid());
		assert(handle.idx < m_uniforms.size());

		UniformInfo& info = m_uniforms[handle.idx];
		m_memory.free(info.data);
	}
}