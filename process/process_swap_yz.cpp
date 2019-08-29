#include "process.h"

void process_swap_yz(process_t& process) {
	float* vertices_start = &(process.tinyobj_attrib.vertices[0]);
	float* vertices_end = vertices_start + process.tinyobj_attrib.vertices.size();

	for (float* vertex = vertices_start; vertex != vertices_end; vertex += 3)
	{
		std::swap(vertex[1], vertex[2]);
	}

	for (uint32 i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];
		if (mesh->mPrimitiveTypes & (aiPrimitiveType_LINE | aiPrimitiveType_POINT)) continue;

		for (uint32 i = 0; i < mesh->mNumVertices; i++)
		{
			auto &v = mesh->mVertices[i];
			auto temp = v.y;
			v.y = v.z;
			v.z = temp;
		}
	}
}