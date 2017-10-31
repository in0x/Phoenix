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
	GLenum err(glGetError());

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

#include <vector>

void err_cb(GLenum        source,
			GLenum        type,
			GLuint        id,
			GLenum        severity,
			GLsizei       length,
			const GLchar* message,
			GLvoid*       userParam)
{
	return;
}

void GetFirstNMessages(GLuint numMsgs)
{
	/*
	
	if (glDebugMessageControlARB != NULL) {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
		glDebugMessageCallbackARB((GLDEBUGPROCARB)err_cb, NULL);
	}

	*/

	GLint maxMsgLen = 0;
	glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &maxMsgLen);

	std::vector<GLchar> msgData(numMsgs * maxMsgLen);
	std::vector<GLenum> sources(numMsgs);
	std::vector<GLenum> types(numMsgs);
	std::vector<GLenum> severities(numMsgs);
	std::vector<GLuint> ids(numMsgs);
	std::vector<GLsizei> lengths(numMsgs);

	GLuint numFound = glGetDebugMessageLog(numMsgs, msgData.size(), &sources[0], &types[0], &ids[0], &severities[0], &lengths[0], &msgData[0]);

	sources.resize(numFound);
	types.resize(numFound);
	severities.resize(numFound);
	ids.resize(numFound);
	lengths.resize(numFound);

	std::vector<std::string> messages;
	messages.reserve(numFound);

	std::vector<GLchar>::iterator currPos = msgData.begin();
	for (size_t msg = 0; msg < lengths.size(); ++msg)
	{
		messages.push_back(std::string(currPos, currPos + lengths[msg] - 1));
		currPos = currPos + lengths[msg];
	}

	return;
}