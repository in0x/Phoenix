#pragma once
#include <vector>
#include <string>

namespace Phoenix
{
	float strToFloat(const char* string);
	int strToInt(const char* string);

	bool compare(const char* token, const char* other);
	size_t find(const char* token, const char* toFind, size_t pos = 0);
	
	std::vector<char*> tokenize(char* string, const char*delimiter);
	size_t subStrCount(const char* string, const char* substr);
}