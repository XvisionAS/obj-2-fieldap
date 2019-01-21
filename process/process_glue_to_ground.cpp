#include "process.h"

void process_glue_to_ground(process_t& process) 
{
	for (uint i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];
		if (mesh->mPrimitiveTypes & (aiPrimitiveType_LINE | aiPrimitiveType_POINT)) continue;

		for (uint m = 0; m < mesh->mNumVertices; m++)
		{
			auto &v = mesh->mVertices[m];
			v.z -= process.min.z;
		}
	}
}