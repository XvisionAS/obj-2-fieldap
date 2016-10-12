#include "process.h"
static void get_diffuse_from_tinyobj_material(process_t& process, int material, char* buffer) {
	float *diffuse = process.tinyobj_materials[material].diffuse;
	int			diffuse_as_int[3] = {
		(int)(diffuse[0] * 255),
		(int)(diffuse[1] * 255),
		(int)(diffuse[2] * 255),
	};
	sprintf(buffer, "#%.2X%.2X%.2X", diffuse_as_int[0], diffuse_as_int[1], diffuse_as_int[2]);
}


void process_output_svg_triangle(process_t& process) {
	std::sort(
		process.triangles.begin(),
		process.triangles.end(),
		[](const triangle_t& a, triangle_t& b) -> bool { return a.max_z < b.max_z; }
	);

	std::ofstream svg;
	
	svg.open(process.file_name_without_ext + ".triangle.svg");
	svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"" << process.min.x << " " << process.min.y << " " << (process.max.x - process.min.x) << " " << (process.max.y - process.min.y) << "\" >";
	
	int previous_material = -1;
	int id = 0;
	for (auto triangle : process.triangles) {
		if (triangle.valid) {
			int need_change_material = previous_material != triangle.material;
			if (need_change_material) {
				if (previous_material != -1) {
					svg << "</g>";
				}
				previous_material = triangle.material;
				char color[16];
				get_diffuse_from_tinyobj_material(process, triangle.material, color);
				svg << "<g id=\"" << id << "\" stroke=\"black\" fill=\"" << color << "\" stroke-width=\"0.01\">";
				id++;
			}
			svg << "<polygon points=\"";
			svg << process.vertices[triangle.a].x << "," << process.vertices[triangle.a].y << " ";
			svg << process.vertices[triangle.b].x << "," << process.vertices[triangle.b].y << " ";
			svg << process.vertices[triangle.c].x << "," << process.vertices[triangle.c].y << " ";
			svg << "\"/>";
			svg << "<!--" << triangle.max_z << "-->";
		}
	}
	svg << "</g></svg>";
}
