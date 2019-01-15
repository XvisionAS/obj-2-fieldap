#include "process.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

bool process_load_obj(process_t& process) {
	/*return*/ 
	tinyobj::LoadObj(
	 	&process.tinyobj_attrib,
	 	&process.tinyobj_shapes,
	 	&process.tinyobj_materials,
	 	&process.tinyobj_err,
	 	process.file_name.c_str(),
	 	process.file_path.c_str());

	process.scene = process.importer.ReadFile(process.file_name.c_str(), aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_PreTransformVertices);

	if (!process.scene){
		printf("%s\n", process.importer.GetErrorString());
		return false;
	}else{
		return true;
	}

}
