#include "process.h"


void process_compute_min_max(process_t& process) {
	process.min.set(std::numeric_limits<float>::max());
	process.max.set(-std::numeric_limits<float>::max());


	for (const auto& shape : process.tinyobj_shapes) {
		size_t pos = shape.name.find("tag_");
		if (pos == 0) {
			continue;
		}
		size_t		faces_count = shape.mesh.num_face_vertices.size();
		int			offset = 0;
		for (size_t face = 0; face < faces_count; ++face) {
			int num_face_vertices = shape.mesh.num_face_vertices[face];

			for (int i = 0; i < num_face_vertices; ++i) {
				int a = shape.mesh.indices[offset++].vertex_index;
				const auto& v = process.vertices[a];
				process.min.min(v);
				process.max.max(v);
			}
		}
	}
}

