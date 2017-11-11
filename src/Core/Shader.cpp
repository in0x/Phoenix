#include "Shader.hpp"

#include <assert.h>
#include <fstream>

#include <Core/Logger.hpp>
#include <Render/RIDevice.hpp>

namespace Phoenix
{
	std::string loadText(const char* path)
	{
		std::string fileString;
		std::ifstream fileStream(path);

		if (fileStream)
		{
			fileString.assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
			fileString.erase(std::remove_if(fileString.begin(), fileString.end(),
				[](const char c) {
				return !(c >= 0 && c < 128);
			}), fileString.end());

			return fileString;
		}
		else
		{
			Logger::error("Failed to load shader file");
			return nullptr;
		}
	}

	ProgramHandle loadShaderProgram(IRIDevice* renderDevice, char* vsPath, const char* fsPath)
	{
		std::string vsSource = loadText(vsPath);
		VertexShaderHandle vs= renderDevice->createVertexShader(vsSource.c_str());
		assert(vs.isValid());

		std::string fsSource = loadText(fsPath);
		FragmentShaderHandle fs = renderDevice->createFragmentShader(fsSource.c_str());
		assert(fs.isValid());
		
		ProgramHandle program = renderDevice->createProgram(vs, fs);
		assert(program.isValid());

		return program;
	}
}