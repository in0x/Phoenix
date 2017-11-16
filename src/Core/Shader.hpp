#pragma once

#include <Render/RIResourceHandles.hpp>

namespace Phoenix
{
	class IRIDevice;

	ProgramHandle loadShaderProgram(IRIDevice* renderDevice, char* vsPath, const char* fsPath);
}