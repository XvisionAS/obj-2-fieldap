#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>

#include <vector>
#include <list>
#include <string>
#include <map>
#include <algorithm>
#include <fstream>
#include <limits>

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "../externals/tiny_obj_loader.h"
#include "../libs/v3.h"
#include "../libs/rasterizer.h"



typedef unsigned int uint32;

struct triangle_t {
	int			a, b, c;
	int			an, bn, cn;
	float		max_z, min_z;
	int			material; //Change to aiCOlor
	int			valid;
  aiMesh* mesh;
};

typedef std::vector<triangle_t>	triangles_t;

struct process_t {
	std::vector<tinyobj::material_t>	tinyobj_materials;
	std::vector<tinyobj::shape_t>			tinyobj_shapes;
	tinyobj::attrib_t									tinyobj_attrib;
	std::string												tinyobj_err;

	Assimp::Importer                  importer;
  const aiScene                     *scene;

	int															render_tex_size;
	bool														debug_render_to_tga;
	triangles_t											triangles;
	std::vector<point_t>						points;
	std::vector<v3>									vertices;

	std::string											file_name;
	std::string											file_name_without_ext;
	std::string											file_path;
	bool														swap_yz;
	bool														center_xy;
	bool														sort_using_zmax;

	v3															min;
	v3															max;

	float														gamma;
	bool														use_gamma_correction;

	bool														glue;

	float														outline_thickness;
	std::string											outline_color;

	std::map<int, triangles_t>			triangles_per_materials;

	float														svg_scale;
};

void process_from_tinyobj_to_v3(process_t& process);
void process_optimize_mesh(process_t& process);
void process_triangle_occlusion(process_t& process);
void process_remove_degenerate_triangle(process_t& process);
void process_debug_render_mesh_to_tga(process_t& process);
void process_compute_min_max(process_t& process);
void process_transform_points(process_t& process);
void process_split_triangle_by_material(process_t& process);
void process_generate_triangle_list(process_t& process);
bool process_load_obj(process_t& process);
void process_file_path_and_name(process_t& process);
void process_output_svg(process_t& process);
void process_output_svg2(process_t& process);
void process_center_xy(process_t& process);
void process_output_threejs(process_t& process);
void process_output_socket(process_t& process);
void process_swap_yz(process_t& process);
void process_glue_to_ground(process_t& process);
void process_fix_winding(process_t &process);
void process_output_svg_triangles(process_t &process);
void process_backface_culling(process_t& process);

bool is_tag(const std::string& string);