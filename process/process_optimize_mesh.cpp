#include "process.h"

void process_optimize_mesh(process_t& process) {
	float threshold = 0.001f;
	int		welded = 0;
	for (int i = 1; i < process.vertices.size(); ++i) {
		const auto& v = process.vertices[i];
		for (int j = 0; j < i; ++j) {
			const auto vc = v3::sub(process.vertices[j], v);
			if (vc.sqrlen() < threshold) {
				for (auto& triangle : process.triangles) {
					if (triangle.a == i) {
						triangle.a = j;
					}
					if (triangle.b == i) {
						triangle.b = j;
					}
					if (triangle.c == i) {
						triangle.c = j;
					}
				}
				welded++;
				break;
			}			
		}
	}
	std::cout << "x welded " << welded << " point(s)" << std::endl;
}
