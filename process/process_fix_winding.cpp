#include "process.h"

void process_fix_winding(process_t &process) {
	for (auto &triangle : process.triangles) {
		const v3& va = process.vertices[triangle.a];
		const v3& vb = process.vertices[triangle.b];
		const v3& vc = process.vertices[triangle.c];

		v3  vab = v3::sub(vb, va);
		v3  vac = v3::sub(vc, va);
		const float dot = v3::dot(vab, vac);
		if (dot >= 0) {
			std::swap(triangle.b, triangle.c);
		}
	}
}
