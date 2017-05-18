#pragma once
#include "StringTokenizer.hpp"

StringTokenizer::StringTokenizer(const std::string& tokens, const std::string& delimiter)
	: m_string(std::move(tokens)) // TODO: Use move semantics?
	//, m_pTokens()
	, m_tokenLenghts()
{
	tokenize(delimiter);
}

std::string StringTokenizer::operator[](size_t idx)
{
	const char* begin = m_string.c_str();;
	return std::string(begin + m_tokenLenghts[idx]);
}

const char* StringTokenizer::getToken(size_t idx)
{
	return m_string.c_str() + m_tokenLenghts[idx];
}

bool StringTokenizer::compare(size_t idxOfToken, const std::string& other)
{
	return strcmp(getToken(idxOfToken), other.c_str()) == 0;
}

bool StringTokenizer::find(size_t idxOfToken, const std::string& toFind)
{
	const char* search = getToken(idxOfToken);
	const char* token = toFind.c_str();
	size_t tokenSize = toFind.size();

	const char* current = search;

	while ((current = strstr(current, token)) != nullptr)
	{
		if (strncmp(current, token, tokenSize) == 0)
		{
			return true;
		}
	}

	return false;
}

size_t StringTokenizer::size()
{
	return m_tokenLenghts.size();
}

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

float StringTokenizer::tokenToFloat(size_t idxOfToken)
{
	return strToFloat(getToken(idxOfToken));
}

size_t StringTokenizer::subStrCount(const std::string& substr)
{
	size_t count = 0;
	size_t pos = 0;
	while ((pos = m_string.find(substr, pos)) != std::string::npos)
	{
		++count;
		++pos;
	}
	return count; 
}

void StringTokenizer::tokenize(const std::string& delimiter) 
{
	size_t pos = 0;
	const size_t delimiterLength = delimiter.length(); 

	const size_t delimiterCount = subStrCount(delimiter);
	m_tokenLenghts.reserve(delimiterCount);

	const char* first = m_string.c_str();
	m_tokenLenghts.push_back(0);

	while ((pos = m_string.find(delimiter, pos)) != std::string::npos)
	{
		const char* thisTokenBegin = &m_string[pos] + delimiterLength;

		m_tokenLenghts.push_back(thisTokenBegin - first);
		
		for (size_t i = pos; i < pos + delimiterLength; ++i)
		{
			m_string[i] = '\0';
		}
	}
}