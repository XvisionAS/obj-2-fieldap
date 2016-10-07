#include "process.h"
#include "../externals/json.hpp"

void process_output_threejs(process_t& process) {
	json::JSON json;
	auto& metadata = json["metadata"];
	metadata["formatVersion"] = 3.1;
	metadata["generatedBy"] = "obj-2-fieldap";	
	metadata["materials"] = (int)process.tinyobj_materials.size();	
	metadata["uvs"] = 0;
	metadata["colors"] = 0;
	metadata["normals"] = 0;
	
	json["scale"] = 1;	
	
	auto& materials = json["materials"] = json::Array();
	auto& vertices  = json["vertices"] = json::Array();
	auto& faces = json["faces"] = json::Array();
	
	int material_size = (int)process.tinyobj_materials.size();
	for (int i = 0; i < material_size; ++i) {
		auto& material = materials[i];
		const auto& tinyobj_material = process.tinyobj_materials[i];

		material["DbgIndex"] = i;
		material["DbgName"] = tinyobj_material.name;
		
		material["colorDiffuse"][0] = tinyobj_material.diffuse[0];
		material["colorDiffuse"][1] = tinyobj_material.diffuse[1];
		material["colorDiffuse"][2] = tinyobj_material.diffuse[2];

		material["colorSpecular"][0] = tinyobj_material.specular[0];
		material["colorSpecular"][1] = tinyobj_material.specular[1];
		material["colorSpecular"][2] = tinyobj_material.specular[2];
		
		material["illumination"]		= tinyobj_material.illum;
		material["opacity"]				= tinyobj_material.dissolve;
		material["transparent"]		= tinyobj_material.dissolve < (1 - std::numeric_limits<float>::epsilon());
		material["specularCoef"]		= tinyobj_material.shininess;
	}
	
	int									vertex_count = 0;
	int									face_count = 0;
	std::map<int, int>		remapper;

	for (const auto& shape : process.tinyobj_shapes) {
		if (shape.name.find("tag_") == 0) {
			continue;
		}
		size_t offset = 0;
		size_t faces_size = shape.mesh.num_face_vertices.size();
		for (size_t face = 0; face < faces_size; ++face) {
			int num_face_vertices = shape.mesh.num_face_vertices[face];
			int material = shape.mesh.material_ids[face];

			auto		add_vertex = [&](int index) -> int {
				auto it = remapper.find(index);
				if (it != remapper.end()) {
					return it->second;
				}
				const auto& v = process.vertices[index];
				int ret = vertices.length() / 3;				
				if (process.swap_yz) {
					vertices.append(v.x), vertices.append(v.z), vertices.append(v.y);
				}
				else {
					vertices.append(v.x), vertices.append(v.y), vertices.append(v.z);
				}				
				remapper.insert(std::make_pair(index, ret));
				return ret;
			};
			
			/* for every face:
			# face type
			# vertex indices
			# material index
			# face uvs index
			# face vertex uvs indices
			# face normal index
			# face vertex normals indices
			# face color index
			# face vertex colors indices
			face type: 
			faceType = 0
			faceType = setBit(faceType, 0, not isTriangle)
			faceType = setBit(faceType, 1, hasMaterial)
			faceType = setBit(faceType, 2, hasFaceUvs)
			faceType = setBit(faceType, 3, hasFaceVertexUvs)
			faceType = setBit(faceType, 4, hasFaceNormals)
			faceType = setBit(faceType, 5, hasFaceVertexNormals)
			faceType = setBit(faceType, 6, hasFaceColors)
			faceType = setBit(faceType, 7, hasFaceVertexColors)
			*/

			if (num_face_vertices == 3) {
				faces.append(2); // face type -> triangle with material
				faces.append(add_vertex(shape.mesh.indices[offset + 0].vertex_index));
				faces.append(add_vertex(shape.mesh.indices[offset + 1].vertex_index));
				faces.append(add_vertex(shape.mesh.indices[offset + 2].vertex_index));
			}
			else if (num_face_vertices == 4) {
				faces.append(3); faces.append(2); // face type -> face with material
				faces.append(add_vertex(shape.mesh.indices[offset + 0].vertex_index));
				faces.append(add_vertex(shape.mesh.indices[offset + 1].vertex_index));
				faces.append(add_vertex(shape.mesh.indices[offset + 2].vertex_index));
				faces.append(add_vertex(shape.mesh.indices[offset + 3].vertex_index));				
			}
			faces.append(material);			
			offset += num_face_vertices;			
		}	
		vertex_count += (int) offset;
		face_count += (int) faces_size;
	}

	metadata["faces"] = face_count;
	metadata["vertices"] = vertex_count;

	std::ofstream threejs;
	threejs.open(process.file_name_without_ext + ".json");
	threejs << json;
	threejs.close();
}

