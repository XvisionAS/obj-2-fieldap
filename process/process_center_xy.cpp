#include "process.h"

void process_center_xy(process_t& process) 
{
	v3 center = v3(0.0f);
	uint total_verts = 0;
	for (uint i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];
		if (mesh->mPrimitiveTypes & (aiPrimitiveType_LINE | aiPrimitiveType_POINT)) continue;

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
		if (mesh->mPrimitiveTypes & (aiPrimitiveType_LINE | aiPrimitiveType_POINT)) continue;

		for (uint m = 0; m < mesh->mNumVertices; m++)
		{
			auto &v = mesh->mVertices[m];
			v.x-=center.x;
			v.y-=center.y;
		}
	}
}