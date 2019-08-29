#include "process.h"
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

void process_output_socket(process_t& process)
{
	json::JSON json;

	for (uint32 i = 0; i < process.scene->mNumMeshes; i++)
	{
		auto mesh = process.scene->mMeshes[i];

		if (!is_tag(mesh->mName.C_Str()))
		{
			continue;
		}

		auto split = split_string(mesh->mName.C_Str(), "_");

		if (split.empty()) {
			continue;
		}

		std::string name = split[1];
		json::JSON	tag;

		tag["name"] = name;
		if (split.size() > 2) {
			tag["types"] = json::Array(
				std::atoi(split[2].c_str())
			);
		}

		//Finding center
		v3 center = v3(0.0f);
		for (uint32 m = 0; m < mesh->mNumVertices; m++)
		{
			auto &v = mesh->mVertices[m];
			center.add(v3(v.x, v.y, v.z));
		}
		center.mul(1.0f / (float)(mesh->mNumVertices));


		tag["x"] = center.x;
		if (process.swap_yz)
		{
			tag["y"] = center.y;
			tag["z"] = center.z;
		}
		else 
		{
			tag["y"] = center.z;
			tag["z"] = center.y;
		}
		json.append(tag);
	}

	std::ofstream socket;
	socket.open(process.file_name_without_ext + ".sockets");
	socket << json;
	socket.close();
}