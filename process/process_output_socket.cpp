#include "process.h"
#include "../externals/json.hpp"


std::vector<std::string> split_string(const std::string& str, const std::string& delimiters) {
	std::vector<std::string> split;
	size_t p = 0, c;
	do {
		c = str.find_first_of(delimiters, p);
		split.push_back(
			str.substr(
				p,
				((c == std::string::npos) ? str.size() : c) - p
			)
		);
		p = c + 1;
	} while (c != std::string::npos);
	return split;
}


void process_output_socket(process_t& process) {
	json::JSON json;	
	for (const auto& shape : process.tinyobj_shapes) {
		if (!is_tag(shape.name)) {
			continue;
		}
		auto split = split_string(shape.name, "_");
		
		if (split.empty()) {
			continue;
		}

		std::string name = split[1];
		json::JSON	tag;

		
		tag["name"] = name;
		if (split.size() > 2) {
			tag["types"] = json::Array(
				std::atoi(split[2].c_str())
			);
		}		
		// compute center
		size_t		faces_count = shape.mesh.num_face_vertices.size();		
		v3			center(0.0f);
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