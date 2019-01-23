#include "process.h"

void process_backface_culling(process_t& process) 
{	
	const v3 camera(0.0f, process.swap_yz ? -1.0f : 0.0f, process.swap_yz ? 0.0f : -1.0f);

	triangles_t triangles;
	triangles.reserve(process.triangles.size());
	for (auto& triangle : process.triangles) 
	{		
		bool valid = true;
		if (triangle.an != -1)
		{

			const v3 normal(
				triangle.mesh->mNormals[triangle.an].x,
				triangle.mesh->mNormals[triangle.an].y,
				triangle.mesh->mNormals[triangle.an].z
			);

			valid = v3::dot(normal, camera) < 0;
		}


		if (valid) 
		{
			triangles.push_back(triangle);
		}
	}

	process.triangles.swap(triangles);
}
