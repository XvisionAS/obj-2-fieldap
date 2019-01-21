#include "process.h"
#include <cassert>

void process_generate_triangle_list(process_t& process) 
{
	size_t face_count = 0;
	for (uint i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];
		if (mesh->mPrimitiveTypes & (aiPrimitiveType_LINE | aiPrimitiveType_POINT)) continue;

		if (!is_tag(mesh->mName.C_Str()))
		{
			face_count += mesh->mNumFaces;
		}
	}

	process.triangles.resize(face_count);

	size_t triangle_index = 0;
	size_t total_vertex_count = 0;
	for (uint mesh_it = 0; mesh_it < process.scene->mNumMeshes; mesh_it++)
	{
		auto mesh = process.scene->mMeshes[mesh_it];
		if (mesh->mPrimitiveTypes & (aiPrimitiveType_LINE | aiPrimitiveType_POINT)) continue;

		if (is_tag(mesh->mName.C_Str()))
		{
			continue;
		}

		size_t faces_count = mesh->mNumFaces;

		for (size_t face_it = 0; face_it < faces_count; ++face_it)
		{
			auto   face              = mesh->mFaces[face_it];
			size_t num_face_vertices = face.mNumIndices;
			auto   material          = mesh->mMaterialIndex;

			assert(num_face_vertices == 3);

			if (num_face_vertices == 3)
			{
				auto& triangle = process.triangles[triangle_index++];

				triangle.a = total_vertex_count + face.mIndices[0]; //Vertex indexes are for process.vertices
				triangle.b = total_vertex_count + face.mIndices[1];
				triangle.c = total_vertex_count + face.mIndices[2];

				triangle.an = face.mIndices[0]; //Normal are for assimp mesh->mNormals
				triangle.bn = face.mIndices[1];
				triangle.cn = face.mIndices[2];

				triangle.max_z = std::max(mesh->mVertices[face.mIndices[0]].z, std::max(mesh->mVertices[face.mIndices[1]].z, mesh->mVertices[face.mIndices[2]].z));
		
				triangle.material = material;
				triangle.valid = 1;
				triangle.mesh = mesh;

			}

		}

		total_vertex_count += mesh->mNumVertices;
	}
		
	std::sort(
		process.triangles.begin(),
		process.triangles.end(),
		[](const triangle_t& a, triangle_t& b) -> bool { return a.max_z > b.max_z; }
	);
}
