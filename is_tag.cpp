#include "process.h"

#include <string>
#include <cctype>
#include <algorithm>

const static std::string tag("tag_");

bool is_tag(const std::string& string) {
	return std::search(
		string.begin(), string.end(),
		tag.begin(), tag.end(),
		[](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
	) != string.end();
}
