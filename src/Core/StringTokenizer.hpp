#pragma once
#include <vector>
#include <string>

namespace Phoenix
{
	float strToFloat(const char* string);
	int strToInt(const char* string);

	bool compare(const char* token, const char* other);

	// If found, returns the index of toFind in token. Retunrs std::string::npos
	// otherwise. Optionally, pos can be used to start the search at a specific 
	// point within token.
	size_t find(const char* token, const char* toFind, size_t pos = 0);
	
	// Replaces all trailing whitespace in string with the null character.
	void trimTrailingWhitespace(char* string);

	// Modifies string by replacing each occurence of delimiter with 
	// a null character. This allows for each substring to remain in 
	// place while acting like a separate string when used with functions
	// that terminate on encountering the null character.
	// Returns a vector of char* each pointing to the beginning of the next token.
	std::vector<char*> tokenize(char* string, const char* delimiter);

	size_t subStrCount(const char* string, const char* substr);
}