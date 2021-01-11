#include "process.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Exporter.hpp>
#include <assimp/cimport.h>

#include "../externals/json.hpp"


std::vector<std::string> split_string(const std::string& str, const std::string& delimiters)
{
	std::vector<std::string> split;
	size_t p = 0, c;
	do {
		c = str.find_first_of(delimiters, p);
		split.push_back(
			str.substr(
				p,
				((c == std::string::npos) ? str.size() : c) - p
			)
		);
		p = c + 1;
	} while (c != std::string::npos);
	return split;
}

static void assimp_get_transformation(aiNode* node, aiMatrix4x4* matrix)
{
	if (node->mParent)
	{
		assimp_get_transformation(node->mParent, matrix);
	}
	aiMultiplyMatrix4(matrix, &node->mTransformation);
}


void process_node(process_t& process, json::JSON& json, aiNode* parentNode) {
	for (uint32 i = 0; i < parentNode->mNumChildren; i++) {
		auto node = parentNode->mChildren[i];

		process_node(process, json, node);

		if (node->mNumMeshes != 1) {
			continue;
		}

		auto meshIndex = node->mMeshes[0];
		auto mesh = process.scene->mMeshes[meshIndex];

		std::string meshName(mesh->mName.C_Str());
		if (!(meshName.find("tag_") == 0 || meshName.find("docking_") == 0)) {
			continue;
		}

		auto split = split_string(meshName, "_");

		if (split.empty()) {
			continue;
		}

		if (split[0] == "tag") {
			std::string name = split[1];
			json::JSON	tag;

			tag["name"] = name;
			if (split.size() > 2) {
				tag["types"] = json::Array(
					std::atoi(split[2].c_str())
				);
			}

			aiMatrix4x4 matrix;
			aiIdentityMatrix4(&matrix);
			
			assimp_get_transformation(node, &matrix);
			
			aiQuaternion quaternion;
			aiVector3D   position;

			matrix.DecomposeNoScaling(quaternion, position);

			tag["x"] = position.x;
			if (process.swap_yz)
			{
				tag["y"] = position.z;
				tag["z"] = position.y;
			}
			else
			{
				tag["y"] = position.y;
				tag["z"] = position.z;
			}
			json["tags"].append(tag);
		}
		else if (split[0] == "docking") {
			json::JSON	docking;
			auto type = split[1];
			docking["name"] = split[2];

			aiMatrix4x4 matrix;
			aiIdentityMatrix4(&matrix);

			assimp_get_transformation(node, &matrix);

			aiQuaternion quaternion;
			aiVector3D   position;

			matrix.DecomposeNoScaling(quaternion, position);

			docking["translation"]["x"] = position.x;
			docking["translation"]["y"] = position.y;
			docking["translation"]["z"] = position.z;

			docking["rotation"]["x"] = quaternion.x;
			docking["rotation"]["y"] = quaternion.y;
			docking["rotation"]["z"] = quaternion.z;
			docking["rotation"]["w"] = quaternion.w;

			if (type == "male") {
				json["males"].append(docking);
			}
			else {
				json["female"] = docking;
			}

		}
	}
}

void process_output_socket(process_t& process)
{
	json::JSON json;
	json["fileType"] = "futureon-model-infos";
	json["fileVersion"] = "1.0.0";

	auto rootNode = process.scene->mRootNode;
	
	process_node(process, json, rootNode);

	std::ofstream socket;
	socket.open(process.file_name_without_ext + ".sockets");
	socket << json;
	socket.close();
}