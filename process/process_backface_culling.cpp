#include "process.h"

void process_backface_culling(process_t& process) {	
	const v3 camera(0, process.swap_yz ? -1 : 0, process.swap_yz ? 0 : -1);

	triangles_t triangles;

	triangles.reserve(process.triangles.size());
	for (auto& triangle : process.triangles) {
		
		const v3 normal(
			process.tinyobj_attrib.normals[triangle.an * 3],
			process.tinyobj_attrib.normals[triangle.an * 3 + 1],
			process.tinyobj_attrib.normals[triangle.an * 3 + 2]
		);
		const bool valid = v3::dot(normal, camera) < 0;
		if (valid) {
			triangles.push_back(triangle);
		}		
	}
	process.triangles.swap(triangles);
}
