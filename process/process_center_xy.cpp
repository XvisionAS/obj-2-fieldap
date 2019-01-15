#include "process.h"

void process_center_xy(process_t& process) {
	/*float* vertices_start = &(process.tinyobj_attrib.vertices[0]);
	float* vertices_end = vertices_start + process.tinyobj_attrib.vertices.size();

	v3 center(0.0f);
	for (float* vertex = vertices_start; vertex != vertices_end; vertex += 3) {
		center.add(vertex);
	}

	center.mul(1.0f / (float)(process.tinyobj_attrib.vertices.size() / 3));

	for (float* vertex = vertices_start; vertex != vertices_end; vertex += 3) {
		*(vertex + 0) -= center.x;
		*(vertex + 1) -= center.y;
	}*/

	v3 center = v3(0.0f);
	uint total_verts = 0;
	for (uint i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];
		total_verts += mesh->mNumVertices;
		for (uint m = 0; m < mesh->mNumVertices; m++)
		{
			auto &v = mesh->mVertices[m];
			center.add(v3(v.x, v.y, v.z));
		}
	}

	center.mul(1.0f / (float)(total_verts));

	for (uint i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];
		for (uint m = 0; m < mesh->mNumVertices; m++)
		{
			auto &v = mesh->mVertices[m];
			v.x-=center.x;
			v.y-=center.y;
		}
	}


}