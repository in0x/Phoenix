#include "OpenGL.hpp"
#include <cassert>

std::string getGlErrorString()
{
	GLenum err(glGetError());

	switch (err)
	{
	case GL_NO_ERROR:
		return std::string("GL Error: No Error");
	case GL_INVALID_OPERATION:
		assert(false);
		return std::string("INVALID_OPERATION");
	case GL_INVALID_ENUM:
		assert(false);
		return std::string("INVALID_ENUM");
	case GL_INVALID_VALUE:
		assert(false);
		return std::string("INVALID_VALUE");
	case GL_OUT_OF_MEMORY:
		assert(false);
		return std::string("OUT_OF_MEMORY");
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		assert(false);
		return std::string("INVALID_FRAMEBUFFER_OPERATION");
	default:
		return std::string("GL Error: Case not covered");
		assert(false);
	}
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