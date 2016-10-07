#include "process.h"

void process_center_xy(process_t& process) {
	float* vertices_start = &(process.tinyobj_attrib.vertices[0]);
	float* vertices_end = vertices_start + process.tinyobj_attrib.vertices.size();

	v3 center(0.0f);
	for (float* vertex = vertices_start; vertex != vertices_end; vertex += 3) {
		center.add(vertex);
	}
	center.mul(1.0f / (float)(process.tinyobj_attrib.vertices.size() / 3));
	for (float* vertex = vertices_start; vertex != vertices_end; vertex += 3) {
		*(vertex + 0) -= center.x;
		*(vertex + 1) -= center.y;
	}
}