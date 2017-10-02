#include "process.h"

void process_split_triangle_by_material(process_t& process) {
	for (const auto& triangle : process.triangles) {
		if (triangle.valid) {
			
			if (triangle.material != -1) {				
				process.triangles_per_materials[triangle.material].push_back(triangle);
			}			
		}
	}
}