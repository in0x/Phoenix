#pragma once

#include <vector>
#include <string>

namespace Phoenix
{
	std::vector<std::string> getFilesInDirectory(const char* path);

	bool hasExtension(const char* file, const char* ext);
}