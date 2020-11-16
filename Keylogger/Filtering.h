#pragma once
#include <string>
#include <regex>
#include <vector>

class Filter {
private:
	std::vector<std::wregex> patterns;	
public:
	Filter(std::vector<std::wstring> patterns);
	void AddFilter(std::wstring regexPattern);
	bool Check(std::wstring text);
};