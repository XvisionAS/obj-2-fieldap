#include "process/process.h"
#include "externals/cmdline.h"

void parse_command_line(int ac, char** av, cmdline::parser& cmdparser) {
	cmdparser.add<int>("render-texture-size",
		'a',
		"Size of the texture, in pixels, that will be use to determine if a face is visible. Bigger is better, but slower",
		false,
		4096);

	cmdparser.add<bool>("swap-yz",
		'b',
		"Swap Y and Z axis",
		false,
		true);

	cmdparser.add<bool>("debug_render_to_tga",
		'c',
		"DEBUG: Use the software rasterizer to render the object to TGA",
		false,
		false);

	cmdparser.parse_check(ac, av);
}

int main(int ac, char** av) {
	cmdline::parser cmdparser;

	parse_command_line(ac, av, cmdparser);
	for (auto& input : cmdparser.rest()) {
		process_t process;

		process.swap_yz             = cmdparser.get<bool>("swap-yz");
		process.debug_render_to_tga = cmdparser.get<bool>("debug_render_to_tga");
		process.render_tex_size     = cmdparser.get<int>("render-texture-size");

		process.file_name = input;

		if (!process_load_obj(process)) {
			continue;
		}

		process_compute_min_max(process);		
		process_transform_points(process);
		process_generate_triangle_list(process);
		// process_backface_culling(process);
		// triangle_occlusion_null(process);
		
		process_triangle_occlusion(process);			
				
		process_split_triangle_by_material(process);
		
		// optimize_mesh(process);		
		process_remove_degenerate_triangle(process);
		if (process.debug_render_to_tga) {
			process_debug_render_mesh_to_tga(process);
		}

		process_output_svg(process);
	}
}
