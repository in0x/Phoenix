#pragma once
#include <vector>
#include <string>

float strToFloat(const char* string);

class StringTokenizer
{
public:
	StringTokenizer(const std::string& string, const std::string& delimiter);	
	std::string operator[](size_t idx);
	bool compare(size_t idxOfToken, const std::string& other);	
	bool find(size_t idxOfToken, const std::string& toFind);
	size_t size();
	float tokenToFloat(size_t idxOfToken);

private:
	std::string m_string;
	//std::vector<const char*> m_pTokens;
	std::vector<int> m_tokenLenghts;

	void tokenize(const std::string& delimiter);
	size_t subStrCount(const std::string& substr);
	const char* getToken(size_t idx);
};