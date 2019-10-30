#include "process.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Exporter.hpp>
#include <assimp/cimport.h>
#include <sstream>

static aiNode* assimp_find_node(aiNode* node, size_t meshIndex)
{

	for (size_t i = 0; i < node->mNumMeshes; ++i) {
		if (node->mMeshes[i] == meshIndex) {
			return node;
		}
	}

	for (size_t i = 0; i < node->mNumChildren; i++) {
		aiNode *found = assimp_find_node(node->mChildren[i], meshIndex);
		if (found) {
			return found;
		}

	}
	return NULL;

}

static void assimp_get_transformation(aiNode* node, aiMatrix4x4* matrix)
{
	if (node->mParent)
	{
		assimp_get_transformation(node->mParent, matrix);
	}
	aiMultiplyMatrix4(matrix, &node->mTransformation);
}

bool process_load_obj(process_t& process) 
{

	process.scene = process.importer.ReadFile(process.file_name.c_str(), 
	  aiProcess_Triangulate 
	| aiProcess_GenNormals 
	| aiProcess_JoinIdenticalVertices
	| aiProcess_FindDegenerates
	| aiProcess_FindInvalidData
	| aiProcess_SortByPType);

	

	for (size_t i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];

		if (mesh->mPrimitiveTypes & (aiPrimitiveType_LINE | aiPrimitiveType_POINT))
		{
			continue;
		}

		aiMatrix4x4 matrix;
		aiIdentityMatrix4(&matrix);
		auto node = assimp_find_node(process.scene->mRootNode, i);
		if (!node)
		{
			continue;
		}
		assimp_get_transformation(node, &matrix);
		
		// SO ... we are changing the vertices directly in assimp scene export, as we do not want 
		// to the same matrice operation everywhere.
		// which also mean that we should reset all transformation from the nodes. But since we are not using it anyway ...

		for (size_t m = 0; m < mesh->mNumVertices; m++)
		{
			auto *v = mesh->mVertices + m;
			aiTransformVecByMatrix4(v, &matrix);
		}

	}
	//Assimp::Exporter exporter;
	//for (size_t i = 0; i < exporter.GetExportFormatCount(); ++i) {
	//	auto descr = exporter.GetExportFormatDescription(i);
	//	std::cout << descr->id << std::endl;
	//}

	//exporter.Export(process.scene, "fbx", process.file_path + "\\out.fbx");
	//exporter.Export(process.scene, "glb2", process.file_path + "\\out.glb");

	if (!process.scene)
	{
		printf("Importer error: %s\n", process.importer.GetErrorString());
		return false;
	}
	else
	{
		return true;
	}

}


void process_output_obj(process_t& process) 
{
	std::ofstream obj;
	obj.open(process.file_name_without_ext + ".obj");
	for (auto v : process.vertices)
	{
		obj.precision(4);
		obj << std::fixed;
		obj << "v " << v.x << " " << v.y << " " << v.z << std::endl;		
	}

	for (auto t : process.triangles) {
		obj << "f " << t.a << " " << t.b << " " << t.c << std::endl;
	}

}