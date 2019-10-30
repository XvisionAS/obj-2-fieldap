#include "process.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

void process_from_assimp_to_v3(process_t& process) {
	
	int total_verts = 0;
	for (size_t i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];
		if (mesh->mPrimitiveTypes & (aiPrimitiveType_LINE | aiPrimitiveType_POINT)) continue;

		total_verts += mesh->mNumVertices;
	}
	
	process.vertices.reserve(total_verts);

	for (size_t i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];

		if (mesh->mPrimitiveTypes & (aiPrimitiveType_LINE | aiPrimitiveType_POINT))
		{
			continue;
		}
		
		if (is_tag(mesh->mName.C_Str()))
		{
			continue;
		}

		/*auto node = assimp_find_node(process.scene, )*/

		for (size_t m = 0; m < mesh->mNumVertices; m++)
		{
			const auto &v = mesh->mVertices[m];

			v3 vert(v.x, v.y, v.z);
			process.vertices.push_back(vert);
		}
	}

}
