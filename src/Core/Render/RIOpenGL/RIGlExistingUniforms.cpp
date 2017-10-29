#include "RIGlExistingUniforms.hpp"
#include "RIResourcesOpenGL.hpp"

#include "../../Logger.hpp"
#include <assert.h>

namespace Phoenix
{
	void GlExisitingUniforms::registerUniform(const char* name, GLuint programID, GLuint location, GLint numElements)
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

		m_existingUniforms.emplace(hashValue, GlUniform{ location, numElements });
	}

	bool GlExisitingUniforms::getUniformIfExisting(FNVHash uniformHash, GLuint programID, GlUniform& outUniform) const
	{
		FNVHash hashWithProgram	= hashBytes(&programID, sizeof(GLuint), uniformHash);
		auto uniformIt = m_existingUniforms.find(hashWithProgram);

		if (m_existingUniforms.end() == uniformIt)
		{
			return false;
		}

		outUniform = m_existingUniforms.at(hashWithProgram);
		return true;
	}
}