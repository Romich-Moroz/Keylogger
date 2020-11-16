#include "Filtering.h"
#pragma once

Filter::Filter(std::vector<std::wstring> patterns) {
	for (int i = 0; i < patterns.size(); i++) {
		AddFilter(patterns[i]);
	}
}

void Filter::AddFilter(std::wstring regexPattern) {
	this->patterns.push_back(std::wregex(regexPattern, std::regex_constants::icase));
}

bool Filter::Check(std::wstring text) {
	for (int i = 0; i < this->patterns.size(); i++) {
		std::wsmatch tmp;		
		if (std::regex_search(text, tmp, this->patterns[i]))
			return true;
	}
	return false;	
}
