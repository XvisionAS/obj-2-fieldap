#include "process.h"

void process_backface_culling(process_t& process) {	
	const v3 camera(0.0f, process.swap_yz ? -1.0f : 0.0f, process.swap_yz ? 0.0f : -1.0f);

	int total_valid = 0;
	int total_unvalid = 0;

	triangles_t triangles;

	triangles.reserve(process.triangles.size());
	for (auto& triangle : process.triangles) {		
		bool valid = true;
		if (triangle.an != -1) {
			/*const v3 normal(
				process.tinyobj_attrib.normals[triangle.an * 3],
				process.tinyobj_attrib.normals[triangle.an * 3 + 1],
				process.tinyobj_attrib.normals[triangle.an * 3 + 2]
			);*/

			const v3 normal(
				triangle.mesh->mNormals[triangle.an].x,
				triangle.mesh->mNormals[triangle.an].y,
				triangle.mesh->mNormals[triangle.an].z
			);

			valid = v3::dot(normal, camera) < 0;
		}
		if (valid) {
			total_valid++;
			triangles.push_back(triangle);
		}else total_unvalid++;
	}

	printf("total_unvalid:%d\n", total_unvalid);
	printf("total_valid:%d\n", total_valid);

	process.triangles.swap(triangles);
}
