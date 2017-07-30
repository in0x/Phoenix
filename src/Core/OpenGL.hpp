#pragma once

#include "glew/glew.h"
#include <string>

std::string getGlErrorString();

void err_cb(GLenum        source,
	GLenum        type,
	GLuint        id,
	GLenum        severity,
	GLsizei       length,
	const GLchar* message,
	GLvoid*       userParam);

void GetFirstNMessages(GLuint numMsgs);
