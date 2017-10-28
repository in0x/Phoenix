#pragma once

#include "../../glew/glew.h"

const char* getGlErrorString();
bool checkGlError();

void err_cb(GLenum        source,
	GLenum        type,
	GLuint        id,
	GLenum        severity,
	GLsizei       length,
	const GLchar* message,
	GLvoid*       userParam);

void GetFirstNMessages(GLuint numMsgs);
