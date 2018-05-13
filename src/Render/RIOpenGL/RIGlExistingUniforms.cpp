#include "RIGlExistingUniforms.hpp"
#include "RIResourcesOpenGL.hpp"

#include <Core/Logger.hpp>
#include <assert.h>

namespace Phoenix
{
	void GlExisitingUniforms::registerUniform(const char* name, GLuint programID, GLuint location, GLint numElements, GLenum glType)
	{
		HashFNVIterative<> hash;
		hash.add(name, strlen(name));
		hash.add(&programID, sizeof(programID));

		FNVHash hashValue = hash();

		if (m_existingUniforms.end() != m_existingUniforms.find(hashValue))
		{
			Logger::warningf("Hash collision occured while registering uniform %s. This should be investigated", name);
			assert(false);
		}

		m_existingUniforms.emplace(hashValue, GlUniform{ location, numElements, glType });
	}

	bool GlExisitingUniforms::getUniformIfExisting(const char* name, GLuint programID, GlUniform& outUniform) const
	{
		return getUniformIfExisting(HashFNV<const char*>()(name), programID, outUniform);
	}

	bool GlExisitingUniforms::getUniformIfExisting(FNVHash nameHash, GLuint programID, GlUniform& outUniform) const
	{
		FNVHash hashWithProgram = hashBytes(&programID, sizeof(GLuint), nameHash);
		auto uniformIt = m_existingUniforms.find(hashWithProgram);

		if (m_existingUniforms.end() == uniformIt)
		{
			return false;
		}

		outUniform = uniformIt->second;
		return true;
	}
}