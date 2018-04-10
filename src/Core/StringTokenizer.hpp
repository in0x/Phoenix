#pragma once

#include <vector>
#include <stdint.h>

namespace Phoenix
{
	float strToFloat(const char* str);
	int32_t strToInt(const char* str);

	bool compare(const char* str, const char* other);

	// If found, returns the index of toFind in str. Returns -1
	// otherwise. Optionally, pos can be used to start the search   
	// at a specific point within str.
	int32_t findFirst(const char* str, const char* toFind, size_t pos = 0);

	// If found, returns the index of the last occurence of toFind in str.
	// Returns -1 otherwise. Optionally, pos can be used to start the search 
	// at a specific point within str.
	int32_t findLast(const char* str, const char* toFind, size_t pos = 0);
	
	// Replaces all trailing whitespace in string with the null character.
	void trimTrailingWhitespace(char* string);

	// Modifies string by replacing each occurence of delimiter with 
	// a null character. This allows for each substring to remain in 
	// place while acting like a separate string when used with functions
	// that terminate on encountering the null character.
	// Returns a vector of char* each pointing to the beginning of the next token.
	std::vector<char*> tokenize(char* str, const char* delimiter);

	size_t subStrCount(const char* str, const char* substr);
}