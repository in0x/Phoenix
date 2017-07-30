#include "OpenGL.hpp"

std::string getGlErrorString()
{
	GLenum err(glGetError());

	switch (err)
	{
	case GL_NO_ERROR:
		return std::string("GL Error: No Error");
	case GL_INVALID_OPERATION:
		return std::string("INVALID_OPERATION");
	case GL_INVALID_ENUM:
		return std::string("INVALID_ENUM");
	case GL_INVALID_VALUE:
		return std::string("INVALID_VALUE");
	case GL_OUT_OF_MEMORY:
		return std::string("OUT_OF_MEMORY");
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return std::string("INVALID_FRAMEBUFFER_OPERATION");
	default:
		return std::string("GL Error: Case not covered");
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
	std::vector<int> vec;
	vec.push_back(3);
	return;
}

void GetFirstNMessages(GLuint numMsgs)
{
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