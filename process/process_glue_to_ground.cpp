#include "process.h"

void process_glue_to_ground(process_t& process) {
	/*float* vertices_start = &(process.tinyobj_attrib.vertices[0]);
	float* vertices_end = vertices_start + process.tinyobj_attrib.vertices.size();

	for (float* vertex = vertices_start; vertex != vertices_end; vertex += 3) {
		*(vertex + 2) -= process.min.z;
	}*/


	for (uint i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];
		for (uint m = 0; m < mesh->mNumVertices; m++)
		{
			auto &v = mesh->mVertices[m];
			v.z -= process.min.z;
		}
	}
}