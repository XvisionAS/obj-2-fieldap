#include "process.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../externals/stb_image_write.h"

void process_debug_render_mesh_to_tga(process_t& process) 
{
	frame_buffer_t frame_buffer(process.render_tex_size, process.render_tex_size);

	for (const auto &triangle : process.triangles) 
	{
		aiColor3D aiDiffuse(0.f,0.f,0.f);
		process.scene->mMaterials[triangle.material]->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuse);
		int	diffuse = (int)(aiDiffuse.r * 255) | (int)(aiDiffuse.g * 255) << 8 | (int)(aiDiffuse.b * 255) << 16 | 0xff000000;
		//int			diffuse = (int)(color[0] * 255) | (int)(color[1] * 255) << 8 | (int)(color[2] * 255) << 16 | 0xff000000;

		if (triangle.valid)
		{
			rasterize(frame_buffer, process.points[triangle.a], process.points[triangle.b], process.points[triangle.c], diffuse);			
			rasterize(frame_buffer, process.points[triangle.a], process.points[triangle.c], process.points[triangle.b], diffuse);
		}
	}

	stbi_write_tga((process.file_name + ".diffuse.tga").c_str(), process.render_tex_size, process.render_tex_size, 4, &(frame_buffer.pixels[0]));

	float min = std::numeric_limits<float>::max();
	float max = -std::numeric_limits<float>::max();

	for (int i = 0; i < frame_buffer.width * frame_buffer.height; ++i)
	{
		if (frame_buffer.pixels[i] != -1)
		{
			min = std::min(frame_buffer.depths[i], min);
			max = std::max(frame_buffer.depths[i], max);
		}
	}


	std::vector<unsigned char> depth_as_char;
	depth_as_char.resize(frame_buffer.width * frame_buffer.height, 0);
	for (int i = 0; i < frame_buffer.width * frame_buffer.height; ++i)
	{
		if (frame_buffer.pixels[i] != -1) {
			depth_as_char[i] = (unsigned char)(((frame_buffer.depths[i] - min) / (max - min)) * 255);
		}
	}
	stbi_write_tga((process.file_name + ".depth.tga").c_str(), process.render_tex_size, process.render_tex_size, 1, &(depth_as_char[0]));
}