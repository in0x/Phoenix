#pragma once

#include <vector>
#include <string>

namespace Phoenix
{
	// Returns the names of all fils in this directory.
	std::vector<std::string> getFilesInDirectory(const char* path);

	// Returns whether the given file path ends in ext.
	bool hasExtension(const char* file, const char* ext);

	// Returns the location of the first char of the file name in path.
	// Tries to locate the last of in the following order: "/", "\"
	// If none of these is found, returns 0.
	int32_t fileNameFromPath(const char* path);
}