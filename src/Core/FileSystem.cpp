#include "FileSystem.hpp"

#include <Core/StringTokenizer.hpp>

#include <ThirdParty/dirent/dirent.h>

namespace Phoenix
{
	std::vector<std::string> getFilesInDirectory(const char* path)
	{
		DIR *dir;

		std::vector<std::string> files;

		dir = opendir(path);

		if (dir == nullptr)
		{
			return files;
		}
		
		dirent *ent;

		while ((ent = readdir(dir)) != NULL)
		{
			files.emplace_back(ent->d_name);
		}

		closedir(dir);		

		return files;
	}

	bool hasExtension(const char* file, const char* ext)
	{
		int32_t fileDot = findLast(file, ".");

		if (-1 == fileDot)
		{
			return false;
		}

		if (strcmp(ext, file + fileDot) != 0)
		{
			return false;
		}

		return true;
	}
}