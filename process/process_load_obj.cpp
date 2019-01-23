#include "process.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

bool process_load_obj(process_t& process) 
{

	process.scene = process.importer.ReadFile(process.file_name.c_str(), 
	  aiProcess_Triangulate 
	| aiProcess_GenNormals 
	| aiProcess_JoinIdenticalVertices
	| aiProcess_FindDegenerates
	| aiProcess_FindInvalidData
	| aiProcess_SortByPType);

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
