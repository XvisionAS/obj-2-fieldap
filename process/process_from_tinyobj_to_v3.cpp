#include "process.h"

void process_from_tinyobj_to_v3(process_t& process) {

	/*float* vertices_start = &(process.tinyobj_attrib.vertices[0]);
	float* vertices_end = vertices_start + process.tinyobj_attrib.vertices.size();
	process.vertices.reserve(process.tinyobj_attrib.vertices.size() / 3);

	for (float* vertex = vertices_start; vertex != vertices_end; vertex += 3) {
		v3 v(vertex);
		process.vertices.push_back(v);
	}*/

	//--
	
	//Get total verts
	int total_verts = 0;
	for (uint i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];
		total_verts += mesh->mNumVertices;
	}

	process.vertices.reserve(total_verts);

	for (uint i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];
		for (uint m = 0; m < mesh->mNumVertices; m++)
		{
			auto &v = mesh->mVertices[m];
			v3 vert(v.x, v.y, v.z);
			process.vertices.push_back(vert);
		}
	}

	// for (v3 i: process.vertices)
	// {
	// printf(" x:%f, y:%f, z:%f \n", i.x, i.y, i.z);
	// }

}
