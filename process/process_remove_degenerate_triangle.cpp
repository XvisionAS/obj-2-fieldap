#include "process.h"

void process_remove_degenerate_triangle(process_t& process) {	
	triangles_t triangles;
	size_t count = 0;
	triangles.reserve(process.triangles.size());	
	for (auto& triangle : process.triangles) {
		if (
			triangle.a == triangle.b
			|| triangle.a == triangle.c
			|| triangle.b == triangle.c) {
			count++;
			triangle.valid = 0;
		}
		else {
			triangles.push_back(triangle);
		}
	}
	std::cout << "x removed " << count << " degenerated triangle(s)" << std::endl;
	process.triangles.swap(triangles);
}

