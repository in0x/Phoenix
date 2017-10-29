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
		void registerUniform(const char* name, GLuint programID, GLuint location, GLint numElements);
		bool getUniformIfExisting(FNVHash uniformHash, GLuint programID, GlUniform& outUniform) const;

	private:
		std::unordered_map<FNVHash, GlUniform> m_existingUniforms;
	};
}