#pragma once

#include "Commands.hpp"
#include "RIContext.hpp"

namespace Phoenix
{
	//namespace SubmitFunctions
	//{
	//	void indexedDraw(RIContext* rc, const void* command)
	//	{
	//		auto dc = static_cast<const Commands::DrawIndexed*>(command);

	//		rc->setState(dc->state);
	//		rc->drawIndexed(dc->vertexBuffer, dc->indexBuffer, dc->primitives, dc->count, dc->start);
	//	}

	//	void linearDraw(RIContext* rc, const void* command)
	//	{
	//		auto dc = static_cast<const Commands::DrawLinear*>(command);

	//		rc->setState(dc->state);
	//		rc->drawLinear(dc->vertexBuffer, dc->primitives, dc->count, dc->start);
	//	}
	//
	//	void clearTarget(RIContext* rc, const void* command)
	//	{
	//		auto dc = static_cast<const Commands::ClearTarget*>(command);
	//		rc->clearRenderTarget(dc->handle, dc->toClear, dc->color);
	//	}

	//	void vertexBufferCreate(RIContext* rc, const void* command)
	//	{
	//		auto dc = static_cast<const Commands::CreateVertexBuffer*>(command);
	//		rc->createVertexBuffer(dc->handle, dc->format);
	//	}

	//	void indexBufferCreate(RIContext* rc, const void* command)
	//	{
	//		auto dc = static_cast<const Commands::CreateIndexBuffer*>(command);
	//		rc->createIndexBuffer(dc->handle, dc->size, dc->count, dc->data);
	//	}

	//	void shaderCreate(RIContext* rc, const void* command)
	//	{
	//		auto dc = static_cast<const Commands::CreateShader*>(command);
	//		rc->createShader(dc->handle, dc->source, dc->shaderType);
	//	}

	//	void programCreate(RIContext* rc, const void* command)
	//	{
	//		auto dc = static_cast<const Commands::CreateProgram*>(command);
	//		rc->createProgram(dc->handle, dc->shaders);
	//	}

	//	void textureCreate(RIContext* rc, const void* command)
	//	{
	//		auto dc = static_cast<const Commands::createTexture*>(command);
	//		rc->createTexture(dc->handle, dc->format, dc->desc, dc->name);
	//	}

	//	void textureUpload(RIContext* rc, const void* command)
	//	{
	//		auto dc = static_cast<const Commands::UploadTexture*>(command);
	//		rc->uploadTextureData(dc->handle, dc->data, dc->width, dc->height);
	//	}

	//	void cubemapUpload(RIContext* rc, const void* command)
	//	{
	//		auto dc = static_cast<const Commands::UploadCubemap*>(command);

	//		for (size_t i = 0; i < 6; ++i)
	//		{
	//			rc->uploadTextureData(dc->handle, dc->data.data[i], dc->width, dc->height);
	//		}
	//	}
	//}

	//const SubmitFptr Commands::DrawIndexed::SubmitFunc = SubmitFunctions::indexedDraw;
	//const SubmitFptr Commands::DrawLinear::SubmitFunc = SubmitFunctions::linearDraw;
	//const SubmitFptr Commands::ClearTarget::SubmitFunc = SubmitFunctions::clearTarget;
	//const SubmitFptr Commands::CreateVertexBuffer::SubmitFunc = SubmitFunctions::vertexBufferCreate;
	//const SubmitFptr Commands::CreateIndexBuffer::SubmitFunc = SubmitFunctions::indexBufferCreate;
	//const SubmitFptr Commands::CreateShader::SubmitFunc = SubmitFunctions::shaderCreate;
	//const SubmitFptr Commands::CreateProgram::SubmitFunc = SubmitFunctions::programCreate;
	//const SubmitFptr Commands::createTexture::SubmitFunc = SubmitFunctions::textureCreate;
	//const SubmitFptr Commands::UploadTexture::SubmitFunc = SubmitFunctions::textureUpload;
	//const SubmitFptr Commands::UploadCubemap::SubmitFunc = SubmitFunctions::cubemapUpload;
}
