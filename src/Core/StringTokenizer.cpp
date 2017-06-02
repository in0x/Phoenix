#pragma once
#include "StringTokenizer.hpp"

namespace Phoenix
{
	float strToFloat(const char* string)
	{
		float real = 0.0f;
		bool neg = false;
		if (*string == '-')
		{
			neg = true;
			++string;
		}

		while (*string >= '0' && *string <= '9')
		{
			real = (real * 10.0f) + (*string - '0');
			++string;
		}

		if (*string == '.')
		{
			float fract = 0.0f;
			int fractLen = 0;
			++string;

			while (*string >= '0' && *string <= '9')
			{
				fract = (fract*10.0f) + (*string - '0');
				++string;
				++fractLen;
			}

			real += fract / std::pow(10.0f, fractLen);
		}
		if (neg)
		{
			real = -real;
		}
		return real;
	}

	int strToInt(const char* string)
	{
		int real = 0;
		bool neg = false;
		if (*string == '-')
		{
			neg = true;
			++string;
		}

		while (*string >= '0' && *string <= '9')
		{
			real = (real * 10) + (*string - '0');
			++string;
		}

		if (neg)
		{
			real = -real;
		}
		return real;
	}

	bool compare(const char* token, const char* other)
	{
		return strcmp(token, other) == 0;
	}

	// Returns first index of substring if found, else std::string::npos;
	size_t find(const char* token, const char* toFind, size_t pos)
	{
		size_t tokenSize = strlen(toFind);

		const char* current = token + pos;

		while ((current = strstr(current, toFind)) != nullptr)
		{
			if (strncmp(current, toFind, tokenSize) == 0)
			{
				return current - token;
			}
		}

		return std::string::npos;
	}

	std::vector<char*> tokenize(char* string, const char* delimiter)
	{
		std::vector<char*> tokens;

		size_t pos = 0;
		const size_t delimiterLength = strlen(delimiter);

		const size_t delimiterCount = subStrCount(string, delimiter) + 1;
		tokens.reserve(delimiterCount);

		char* first = string;
		tokens.push_back(first);

		while ((pos = find(string, delimiter, pos)) != std::string::npos)
		{
			tokens.push_back(first + pos + delimiterLength);

			for (size_t i = pos; i < pos + delimiterLength; ++i)
			{
				string[i] = '\0';
			}

			pos += delimiterLength;
		}

		return tokens;
	}

	size_t subStrCount(const char* string, const char* substr)
	{
		size_t count = 0;
		size_t pos = 0;
		while ((pos = find(string, substr, pos)) != std::string::npos)
		{
			++count;
			++pos;
		}
		return count;
	}
}