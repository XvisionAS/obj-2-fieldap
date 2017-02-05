#include "process.h"
#include "../externals/json.hpp"

void process_output_socket(process_t& process) {
	json::JSON json;

	for (const auto& shape : process.tinyobj_shapes) {
		size_t pos = shape.name.find("tag_");
		if (pos != 0) {
			continue;
		}
		std::string name = shape.name.substr(pos + 4);
		
		json::JSON tag;
		tag["name"] = name;
		// compute center
		size_t		faces_count = shape.mesh.num_face_vertices.size();		
		v3				center(0.0f);
		int			offset = 0;
		for (size_t face = 0; face < faces_count; ++face) {
			int num_face_vertices = shape.mesh.num_face_vertices[face];

			for (int i = 0; i < num_face_vertices; ++i) {
				int a = shape.mesh.indices[offset++].vertex_index;				
				center.add(process.vertices[a]);
			}	
		}
		center.mul(1.0f / (float)offset);

		tag["x"] = center.x;
		if (process.swap_yz) {
			tag["y"] = center.y;
			tag["z"] = center.z;
		}
		else {
			tag["y"] = center.z;
			tag["z"] = center.y;
		}
		json.append(tag);
	}

	std::ofstream socket;
	socket.open(process.file_name_without_ext + ".sockets");
	socket << json;
	socket.close();
}