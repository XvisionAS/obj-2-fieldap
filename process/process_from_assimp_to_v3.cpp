#include "process.h"

void process_from_assimp_to_v3(process_t& process) {
	
	int total_verts = 0;
	for (uint32 i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];
		if (mesh->mPrimitiveTypes & (aiPrimitiveType_LINE | aiPrimitiveType_POINT)) continue;

		total_verts += mesh->mNumVertices;
	}

	process.vertices.reserve(total_verts);

	for (uint32 i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];
		if (mesh->mPrimitiveTypes & (aiPrimitiveType_LINE | aiPrimitiveType_POINT)) continue;

		for (uint32 m = 0; m < mesh->mNumVertices; m++)
		{
			auto &v = mesh->mVertices[m];
			v3 vert(v.x, v.y, v.z);
			process.vertices.push_back(vert);
		}
	}

}
