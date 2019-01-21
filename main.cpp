#include <assert.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "process/process.h"
#include "externals/cmdline.h"

void parse_command_line(int ac, char** av, cmdline::parser& cmdparser) {
	cmdparser.add<int>("render-texture-size",
		'a',
		"Size of the texture, in pixels, that will be use to determine if a face is visible. Bigger is better, but slower",
		false,
		1024);

	cmdparser.add<bool>("swap-yz",
		'b',
		"Swap Y and Z axis",
		false,
		true);

	cmdparser.add<bool>("debug-render-to-tga",
		'c',
		"DEBUG: Use the software rasterizer to render the object to TGA",
		false,
		false);

	cmdparser.add<bool>("center-xy",
		'd',
		"Center object on XY axis",
		false,
		false);

	cmdparser.add<bool>("gamma-correct",
		'e',
		"Use gamma correction when exporting SVG",
		false,
		true);

	cmdparser.add<float>("gamma",
		'f',
		"Gamma value for SVG export",
		false,
		2.2f);

	cmdparser.add<bool>("glue-to-ground",
		'g',
		"Glue object to ground (0)",
		false,
		false);

	cmdparser.add<bool>("sort-using-zmax",
		'h',
		"Sort object by their z max value ( z min by default )",
		false,
		false);

	cmdparser.add<float>("outline-thickness",
		'i',
		"Thickness outline ( default 0.1 )",
		false,
		0.1f);

	cmdparser.add<std::string>("outline-color",
		'j',
		"Color of the outline, in HTML hex format ( default #000 )",
		false,
		"#000");

	cmdparser.add<float>("svg-scale",
		'k',
		"Apply scales to SVG",
		false,
		1.0f);

	cmdparser.parse_check(ac, av);
}


int main(int ac, char** av) {
	cmdline::parser cmdparser;

	parse_command_line(ac, av, cmdparser);
	for (auto& input : cmdparser.rest()) {
		process_t process;

		process.swap_yz              = cmdparser.get<bool>("swap-yz");
		process.debug_render_to_tga  = cmdparser.get<bool>("debug-render-to-tga");
		process.render_tex_size      = cmdparser.get<int> ("render-texture-size");
		process.center_xy            = cmdparser.get<bool>("center-xy");
		process.gamma                = cmdparser.get<float>("gamma");
		process.use_gamma_correction = cmdparser.get<bool>("gamma-correct");
		process.sort_using_zmax      = cmdparser.get<bool>("sort-using-zmax");
		process.glue                 = cmdparser.get<bool>("glue-to-ground");
		process.file_name            = input;
		process.outline_color        = cmdparser.get<std::string>("outline-color");
		process.outline_thickness    = cmdparser.get<float>("outline-thickness");
		process.svg_scale            = cmdparser.get<float>("svg-scale");

		process_file_path_and_name(process);

		if (!process_load_obj(process)) 
		{
			continue;
		}

		if (process.swap_yz) {
			process_swap_yz(process);
		}

		if (process.center_xy) {
			process_center_xy(process);
		}

		process_from_tinyobj_to_v3(process);

		process_compute_min_max(process);
		
		if (process.glue) {
			process_glue_to_ground(process);
		}

		process_transform_points(process);
		process_generate_triangle_list(process);

		process_backface_culling(process);
		process_triangle_occlusion(process);

		if (process.debug_render_to_tga) {
			process_debug_render_mesh_to_tga(process);
		}

		process_output_svg(process);

	}
}
