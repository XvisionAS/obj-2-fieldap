#include "process.h"

void process_from_tinyobj_to_v3(process_t& process) {
	float* vertices_start = &(process.tinyobj_attrib.vertices[0]);
	float* vertices_end = vertices_start + process.tinyobj_attrib.vertices.size();
	process.vertices.reserve(process.tinyobj_attrib.vertices.size() / 3);

	for (float* vertex = vertices_start; vertex != vertices_end; vertex += 3) {
		v3 v(vertex);
		process.vertices.push_back(v);
	}
}
