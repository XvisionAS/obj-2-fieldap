#include "process.h"

bool process_load_obj(process_t& process) {
	return tinyobj::LoadObj(
		&process.tinyobj_attrib,
		&process.tinyobj_shapes,
		&process.tinyobj_materials,
		&process.tinyobj_err,
		process.file_name.c_str(),
		process.file_path.c_str());
}
