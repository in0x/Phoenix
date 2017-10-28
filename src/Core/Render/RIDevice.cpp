#pragma once
#include "RIDevice.hpp"
#include "RIContextOpenGL.hpp"

namespace Phoenix
{
	//IRIDevice::IRIDevice(const RenderInit* initStruct, size_t backingMemoryBytes)
	//	: m_backingMemory(backingMemoryBytes)
	//	, m_uniformData(sizeof(Matrix4), UniformHandle::maxValue(), alignof(Matrix4))
	//{
	//	m_context = alloc<RIContextOpenGL>(m_backingMemory);
	//}

	//IRIDevice::~IRIDevice()
	//{
	//	dealloc(m_backingMemory, m_context);
	//}

	//RIVertexBufferRef IRIDevice::createVertexBuffer(const VertexBufferFormat& format)
	//{
	//	RIVertexBufferRef vb = std::make_unique<RIVertexBuffer>();
	//	
	//	m_context->createVertexBuffer(vb->m_buffer, format);
	//	assert(vb->m_buffer.isValid());
	//	
	//	return vb;
	//}

	//RIIndexBufferRef IRIDevice::createIndexBuffer(size_t size, uint32_t count, const void* data)
	//{
	//	RIIndexBufferRef ib = std::make_unique<RIIndexBuffer>();

	//	m_context->createIndexBuffer(ib->m_buffer, size, count, data);
	//	return ib;
	//}

	//void IRIDevice::createShader(ShaderHandle& handle, const char* source, EShader::Type type)
	//{
	//	m_context->createShader(handle, source, type);
	//}

	//RIVertexShaderRef IRIDevice::createVertexShader(const char* source)
	//{
	//	RIVertexShaderRef vs = std::make_unique<RIVertexShader>();
	//	createShader(vs->m_shader, source, EShader::Vertex);
	//	return vs;
	//}

	//RIFragmentShaderRef IRIDevice::createFragmentShader(const char* source)
	//{
	//	RIFragmentShaderRef fs = std::make_unique<RIFragmentShader>();
	//	createShader(fs->m_shader, source, EShader::Vertex);
	//	return fs;
	//}


	//RIUniform* RIDevice::createUniform(const char* name, EUniform::Type type, const void* data, size_t dataSize)
	//{
	//	/*
	//		Instead of all this delaying, we can directly ask if the uniform exists.
	//	*/

	//	RIUniform* uniform = alloc<RIUniform>(m_allocator);

	//	uniform->m_size = dataSize;
	//	uniform->m_type = type;
	//	uniform->m_nameHash = HashFNV<const char*>()(name);

	//	void* storage = m_uniformData.allocate(dataSize, alignof(s_maxUniformSize));
	//	assert(nullptr != storage, "RIDevice has run out of uniform storage");
	//	memcpy(storage, data, dataSize);
	//	uniform->m_data = storage;

	//	return uniform; // We can now make uniform updates inline again. When the uniform has been set, dealloc the storage and set to nullptr -> signals that have updated the value 
	//}

	//RIRenderTarget* RIDevice::createRenderTarget(RenderTargetDesc desc)
	//{
	//	RIRenderTarget* rt = alloc<RIRenderTarget>(m_allocator);

	//	m_context->createRenderTarget(rt->m_target, desc);
	//	assert(rt->m_target.isValid(), "RenderTarget creation failed");

	//	return rt;
	//}

	//RITexture* RIDevice::createTexture(const TextureDesc& desc, const void* data, ETexture::Format format, const char* samplerName)
	//{
	//}
}