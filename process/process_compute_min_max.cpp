#include "process.h"
#include <cctype>



void process_compute_min_max(process_t& process) 
{
	process.min.set(std::numeric_limits<float>::max());
	process.max.set(-std::numeric_limits<float>::max());

	for (uint32 mesh_it = 0; mesh_it < process.scene->mNumMeshes; mesh_it++)
	{
		auto mesh = process.scene->mMeshes[mesh_it];
		if (mesh->mPrimitiveTypes & (aiPrimitiveType_LINE | aiPrimitiveType_POINT)) continue;

		if (is_tag(mesh->mName.C_Str()))
		{
			continue;
		}

		for (uint32 vert_it = 0; vert_it < mesh->mNumVertices; vert_it++)
		{
			auto& v = mesh->mVertices[vert_it];
			process.min.min(v3(v.x, v.y, v.z));
			process.max.max(v3(v.x, v.y, v.z));
		}
	}

}

