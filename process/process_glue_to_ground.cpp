#include "process.h"

void process_glue_to_ground(process_t& process) 
{
	for (uint32 i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];
		if (mesh->mPrimitiveTypes & (aiPrimitiveType_LINE | aiPrimitiveType_POINT)) continue;

		for (uint32 m = 0; m < mesh->mNumVertices; m++)
		{
			auto &v = mesh->mVertices[m];
			v.z -= process.min.z;
		}
	}
}