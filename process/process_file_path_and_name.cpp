#include "process.h"

void process_file_path_and_name(process_t& process) {
	size_t pos = process.file_name.find_last_of("\\/");
	if (pos != std::string::npos) {
		process.file_path = process.file_name.substr(0, pos + 1);
	}

	size_t pos_dot = process.file_name.find_last_of(".\\/");
	if (pos_dot != std::string::npos && pos != pos_dot) {
		process.file_name_without_ext = process.file_name.substr(0, pos_dot);
	}
}