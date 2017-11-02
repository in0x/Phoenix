#pragma once

#include "OpenGL.hpp"
#include "../../FNVHash.hpp"
#include <unordered_map>

namespace Phoenix
{
	struct GlUniform;

	class GlExisitingUniforms
	{
	public:
		void registerUniform(const char* name, GLuint programID, GLuint location, GLint numElements, GLenum glType);
		bool getUniformIfExisting(const char* name, GLuint programID, GlUniform& outUniform) const;
		bool getUniformIfExisting(FNVHash nameHash, GLuint programID, GlUniform& outUniform) const;

	private:
		std::unordered_map<FNVHash, GlUniform> m_existingUniforms;
	};
}