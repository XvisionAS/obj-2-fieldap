#include "process/process.h"

#include <string>
#include <cctype>
#include <algorithm>

const static std::string tag("tag_");
const static std::string docking("docking_");

bool find(const std::string& in, const std::string& what) {
	return std::search(
		in.begin(), in.end(),
		what.begin(), what.end(),
		[](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
	) != in.end();
}

bool is_tag(const std::string& string) {
	return find(string, tag) || find(string, docking);
}
