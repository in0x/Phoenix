#pragma once
#include "StringTokenizer.hpp"

StringTokenizer::StringTokenizer(const std::string& tokens, const std::string& delimiter)
	: m_string(std::move(tokens)) // TODO: Use move semantics?
	, m_pTokens()
	, m_tokenLenghts()
{
	tokenize(delimiter);
}

std::string StringTokenizer::operator[](size_t idx)
{
	const char* tokenBegin = m_pTokens[idx];
	return std::string(tokenBegin, tokenBegin + m_tokenLenghts[idx]);
}

bool StringTokenizer::compare(size_t idxOfToken, const std::string& other)
{
	return strcmp(m_pTokens[idxOfToken], other.c_str()) == 0;
}

bool StringTokenizer::find(size_t idxOfToken, const std::string& toFind)
{
	return strstr(m_pTokens[idxOfToken], toFind.c_str()) != nullptr;
}

size_t StringTokenizer::size()
{
	return m_pTokens.size();
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
	m_pTokens.reserve(delimiterCount);
	m_tokenLenghts.reserve(delimiterCount);

	m_pTokens.push_back(&m_string[0]);
	
	while ((pos = m_string.find(delimiter, pos)) != std::string::npos)
	{
		const char* lastTokenBegin = m_pTokens.back();
		const char* thisDelimiterBegin = &m_string[pos];

		m_tokenLenghts.push_back(thisDelimiterBegin - lastTokenBegin);
		
		m_pTokens.push_back(thisDelimiterBegin + delimiterLength);
		
		for (size_t i = pos; i < pos + delimiterLength; ++i)
		{
			m_string[i] = '\0';
		}
	}

	const char* last = &m_string.back(); // Wont work if our string is zero sized
	const char* lastTokenBegin = m_pTokens.back() - delimiterLength;
	m_tokenLenghts.push_back(last - lastTokenBegin);
}