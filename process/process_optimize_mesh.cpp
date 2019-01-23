#include "process.h"

void process_optimize_mesh(process_t& process) 
{

	/*
		This function is now used primarily to weld identical vertecies that do not share normals. 
		This way the rasterizer will index all faces with identical vertecies the same way, 
		and is therefore able to do edge detection!
		-Simen 23/1/19
	*/

	int		welded = 0;
	for (int i = 1; i < (int) process.vertices.size(); ++i)
	{
		const auto& v = process.vertices[i];
		for (int j = 0; j < i; ++j) 
		{
			if (process.vertices[j].equals(v))
			{
				for (auto& triangle : process.triangles) 
				{
					if (triangle.a == i) 
					{
						triangle.a = j;
					}
					if (triangle.b == i) 
					{
						triangle.b = j;
					}
					if (triangle.c == i) 
					{
						triangle.c = j;
					}
				}
				welded++;
				break;
			}
		}
	}
	std::cout << "x welded " << welded << " point(s)" << std::endl;
}
