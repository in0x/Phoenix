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

	int32_t fileNameFromPath(const char* path)
	{
		const char* seperators[] = { "/", "\\" };
		
		int32_t location = 0;
		int32_t len = strlen(path);

		for (const char* seperator : seperators)
		{
			int32_t nextlocation = findLast(path, seperator);
			if ((nextlocation != -1) && (nextlocation + 1 < len) && nextlocation > location)
			{
				location = nextlocation + 1;
			}
		}

		return location;
	}

}