#include "process.h"

void process_transform_points(process_t& process) 
{
	size_t length = process.vertices.size();

	process.points.resize(length);
	float width = process.max.x - process.min.x;
	float height = process.max.y - process.min.y;

	for (size_t i = 0; i < length; i++) 
	{
		auto     xyz = process.vertices[i];
		point_t& p   = process.points[i];

		p.x = (int)(0.5f + ((xyz.x - process.min.x) / width) * process.render_tex_size);
		p.y = (int)(0.5f + ((xyz.y - process.min.y) / height) * process.render_tex_size);
		p.z = (float) xyz.z;
	}

}
