#pragma once

#include "StringTokenizer.hpp"

namespace Phoenix
{
	float strToFloat(const char* str)
	{
		float real = 0.f;
		bool neg = false;
		if (*str == '-')
		{
			neg = true;
			++str;
		}

		while (*str >= '0' && *str <= '9')
		{
			real = (real * 10.f) + (*str - '0');
			++str;
		}

		if (*str == '.')
		{
			float fract = 0.f;
			int fractLen = 0;
			++str;

			while (*str >= '0' && *str <= '9')
			{
				fract = (fract*10.f) + (*str - '0');
				++str;
				++fractLen;
			}

			real += fract / std::pow(10.f, fractLen);
		}
		if (neg)
		{
			real = -real;
		}

		if (*str == 'e')
		{
			++str;
			int pow = strToInt(str);
			return real * (std::pow(10.f, pow));
		}

		return real;
	}

	int32_t strToInt(const char* str)
	{
		int32_t real = 0;
		bool neg = false;
		if (*str == '-')
		{
			neg = true;
			++str;
		}

		while (*str >= '0' && *str <= '9')
		{
			real = (real * 10) + (*str - '0');
			++str;
		}

		if (neg)
		{
			real = -real;
		}
		return real;
	}

	bool compare(const char* str, const char* other)
	{
		return strcmp(str, other) == 0;
	}

	// Returns first index of substring if found, else -1;
	int32_t findFirst(const char* str, const char* toFind, size_t pos)
	{
		size_t tokenSize = strlen(toFind);

		const char* current = str + pos;

		while ((current = strstr(current, toFind)) != nullptr)
		{
			if (strncmp(current, toFind, tokenSize) == 0)
			{
				return current - str;
			}
		}

		return -1;
	}

	int32_t findLast(const char* str, const char* toFind, size_t pos)
	{
		/*int32_t current = -1;
		int32_t last = 0;
	
		str += pos;

		do
		{
			last = current;
			current = findFirst(str + pos, toFind);
			pos = last;
		} while (current != -1);

		return last;*/
	
		int32_t current = 0;
		int32_t last = 0;

		str += pos;

		while (current != -1)
		{
			last = current;
			current = findFirst(str, toFind, current + 1);
		}

		return last;
	}

	void trimTrailingWhitespace(char* str)
	{
		size_t i = strlen(str);

		while (i > 0 && str[i - 1] == ' ')
		{
			str[--i] = '\0';
		}
	}

	std::vector<char*> tokenize(char* str, const char* delimiter)
	{
		std::vector<char*> tokens;

		size_t pos = 0;
		const size_t delimiterLength = strlen(delimiter);

		const size_t delimiterCount = subStrCount(str, delimiter) + 1;
		tokens.reserve(delimiterCount);

		char* first = str;
		tokens.push_back(first);

		while ((pos = findFirst(str, delimiter, pos)) != std::string::npos)
		{
			tokens.push_back(first + pos + delimiterLength);

			for (size_t i = pos; i < pos + delimiterLength; ++i)
			{
				str[i] = '\0';
			}

			pos += delimiterLength;
		}

		return tokens;
	}

	size_t subStrCount(const char* str, const char* substr)
	{
		size_t count = 0;
		size_t pos = 0;
		
		while ((pos = findFirst(str, substr, pos)) != std::string::npos)
		{
			++count;
			++pos;
		}
		return count;
	}
}