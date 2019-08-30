#include "process.h"

void process_center_xy(process_t& process) 
{
	v3 center = v3(0.0f);
	uint32 total_verts = 0;
	for (size_t i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];
		if (mesh->mPrimitiveTypes & (aiPrimitiveType_LINE | aiPrimitiveType_POINT))
		{
			continue;
		}

		total_verts += mesh->mNumVertices;
				
		for (size_t m = 0; m < mesh->mNumVertices; m++)
		{
			auto &v = mesh->mVertices[m];
			center.add(v3(v.x, v.y, v.z));
		}
	}

	center.mul(1.0f / (float)(total_verts));
	for (uint32 i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];
		if (mesh->mPrimitiveTypes & (aiPrimitiveType_LINE | aiPrimitiveType_POINT))
		{
			continue;
		}
		
		for (uint32 m = 0; m < mesh->mNumVertices; m++)
		{
			auto &v = mesh->mVertices[m];
			v.x-=center.x;
			v.y-=center.y;
		}
	}
}