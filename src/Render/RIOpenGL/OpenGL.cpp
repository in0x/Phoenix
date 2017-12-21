#include "OpenGL.hpp"
#include <assert.h>

struct GlErrorMessages
{
	static const char* msgNoError;
	static const char* msgInvalidOperation;
	static const char* msgInvalidEnum;
	static const char* msgInvalidValue;
	static const char* msgOutOfMemory;
	static const char* msgInvalidFramebufferOperation;

};

const char* GlErrorMessages::msgNoError = "GL Error: No Error";
const char* GlErrorMessages::msgInvalidOperation = "GL Error: Invalid Operation";
const char* GlErrorMessages::msgInvalidEnum = "GL Error: Invalid Enum";
const char* GlErrorMessages::msgInvalidValue = "GL Error: Invalid Value";
const char* GlErrorMessages::msgOutOfMemory = "GL Error: Out of Memory";
const char* GlErrorMessages::msgInvalidFramebufferOperation = "GL Error: Invalid Framebuffer Operation";

const char* getGlErrorString(bool& bErrorOccured)
{
	GLenum err = glGetError();

	switch (err)
	{
	case GL_NO_ERROR:
		bErrorOccured = false;
		return GlErrorMessages::msgNoError;
	case GL_INVALID_OPERATION:
		assert(false);
		bErrorOccured = true;
		return GlErrorMessages::msgInvalidOperation;
	case GL_INVALID_ENUM:
		assert(false);
		bErrorOccured = true;
		return GlErrorMessages::msgInvalidEnum;
	case GL_INVALID_VALUE:
		assert(false);
		bErrorOccured = true;
		return GlErrorMessages::msgInvalidValue;
	case GL_OUT_OF_MEMORY:
		assert(false);
		bErrorOccured = true;
		return GlErrorMessages::msgOutOfMemory;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		assert(false);
		bErrorOccured = true;
		return GlErrorMessages::msgInvalidFramebufferOperation;
	default:
		bErrorOccured = true;
		assert(false);
		return nullptr;		
	}
}

#define PHI_SLOW_GL_ERROR_CHECK 1

bool checkGlErrorOccured()
{
#if PHI_SLOW_GL_ERROR_CHECK 
	bool bErrorOccured;
	const char* errorMsg = getGlErrorString(bErrorOccured);
	assert(!bErrorOccured);
	return bErrorOccured;
#else
	bool bErrorOccured = glGetError() != GL_NO_ERROR;
	assert(!bErrorOccured);
	return bErrorOccured;
#endif
}
