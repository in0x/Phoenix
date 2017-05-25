#pragma once
#include <vector>
#include <string>

namespace Phoenix
{
	float strToFloat(const char* string);

	class StringTokenizer
	{
	public:
		StringTokenizer(const std::string& string, const std::string& delimiter);

		std::string operator[](size_t idx) const;
		size_t size() const;

		bool compare(size_t idxOfToken, const std::string& other) const;
		bool find(size_t idxOfToken, const std::string& toFind) const;
		float tokenToFloat(size_t idxOfToken) const;

	private:
		std::string m_string;
		std::vector<size_t> m_tokenLenghts;

		const char* getToken(size_t idx) const;
		void tokenize(const std::string& delimiter);
		size_t subStrCount(const std::string& substr) const;
	};
}