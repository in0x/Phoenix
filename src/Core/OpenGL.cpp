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