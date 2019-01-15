#include "process.h"
#include <string.h>

struct edge_info_t {
	int count;
	float min_z, max_z;
};

struct edge_t {
	edge_t(int _a, int _b, int _material) : a(_a), b(_b), material(_material) {}
	int a, b, material;
	bool operator<(const edge_t &that)  const {
		if (a != that.a) {
			return a < that.a;
		} else if (b != that.b) {
			return b < that.b;
		} else if (material != that.material) {
			return material < that.material;
		}
		return false;
	}
};

typedef std::map<edge_t, edge_info_t> edge_infos_t;

struct element_t {
	int								material;
	float							z;
	size_t						id;
	std::vector<v3>		vertices;
};

typedef std::vector<element_t> elements_t;

static	 void build_edge_list(process_t& process, edge_infos_t& edges) {
	for (auto triangle : process.triangles) {
		if (triangle.valid) {
			auto addEdge = [&](int a, int b, const triangle_t& tri) {
				if (a > b) {
					std::swap(a, b);
				}
				auto& e = edges[edge_t(a, b, tri.material)];
				e.count++;
				e.max_z = tri.max_z;
				e.min_z = tri.min_z;
			};
			addEdge(triangle.a, triangle.b, triangle);
			addEdge(triangle.b, triangle.c, triangle);
			addEdge(triangle.c, triangle.a, triangle);
		}
	}
}

static void remove_shared_edge(edge_infos_t& edges) {
	auto iter = edges.begin(), end = edges.end();
	while (iter != end) {
		if (iter->second.count > 1) {
			edges.erase(iter++);
		}
		else {
			++iter;
		}
	}
}

static void build_element_list(process_t& process, edge_infos_t& edges, elements_t& elements) {
	auto iter = edges.begin(), end = edges.end();
	while (iter != end) {
		std::vector<int> stack;
		std::vector<std::pair<float, float> > zminzmax;
		int material = iter->first.material;

		stack.push_back(iter->first.a);
		stack.push_back(iter->first.b);

		zminzmax.push_back(std::make_pair(iter->second.min_z, iter->second.max_z));
		zminzmax.push_back(std::make_pair(iter->second.min_z, iter->second.max_z));

		edges.erase(iter);

		int found = 1;
		while (found) {
			int		to_find = stack.back();

			auto		it = std::find_if(edges.begin(), edges.end(), [=](auto& e) -> bool { return (to_find == e.first.a || to_find == e.first.b) && (e.first.material == material); });
			found = it != edges.end();
			if (found) {
				stack.push_back(it->first.a == to_find ? it->first.b: it->first.a);
				zminzmax.push_back(std::make_pair(it->second.min_z, it->second.max_z));
				edges.erase(it);
			}
		}
		stack.pop_back();

		elements.push_back(element_t());
		element_t& element = elements.back();
		element.id = elements.size();
		element.material = material;
		element.z = process.sort_using_zmax ? -std::numeric_limits<float>::max() : std::numeric_limits<float>::max();
		for (size_t i = 0; i < stack.size(); ++i) {
			const int		index = stack[i];
			const auto	&minmax = zminzmax[i];
			const auto	&a = process.vertices[index];

			element.vertices.push_back(a);

			element.z = process.sort_using_zmax ? std::max(element.z, minmax.second) : std::min(element.z, minmax.first);
		}
		iter = edges.begin();
	}
}

static void get_diffuse_from_tinyobj_material(process_t& process, int material, char* buffer) {
	float *diffuse = process.tinyobj_materials[material].diffuse;
	float actual_diffuse[] = {
		diffuse[0], diffuse[1], diffuse[2]
	};

	if (process.use_gamma_correction) {
		actual_diffuse[0] = pow(actual_diffuse[0], 1.0f / process.gamma);
		actual_diffuse[1] = pow(actual_diffuse[1], 1.0f / process.gamma);
		actual_diffuse[2] = pow(actual_diffuse[2], 1.0f / process.gamma);
	}
	actual_diffuse[0] = std::min(1.0f, std::max(0.0f, actual_diffuse[0]));
	actual_diffuse[1] = std::min(1.0f, std::max(0.0f, actual_diffuse[1]));
	actual_diffuse[2] = std::min(1.0f, std::max(0.0f, actual_diffuse[2]));

	int			diffuse_as_int[3] = {
		(int)(actual_diffuse[0] * 255),
		(int)(actual_diffuse[1] * 255),
		(int)(actual_diffuse[2] * 255),
	};
	sprintf(buffer, "#%.2X%.2X%.2X", diffuse_as_int[0], diffuse_as_int[1], diffuse_as_int[2]);
}

void process_output_svg(process_t& process) {
	edge_infos_t		edges;

	build_edge_list(process, edges);
	remove_shared_edge(edges);

	elements_t elements;
	build_element_list(process, edges, elements);

	std::ofstream svg;
	svg.open(process.file_name_without_ext + ".svg");
	svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"" << process.min.x * process.svg_scale << " " << process.min.y * process.svg_scale << " " << (process.max.x - process.min.x) * process.svg_scale << " " << (process.max.y - process.min.y) * process.svg_scale << "\" >";

	std::sort(elements.begin(), elements.end(), [](auto a, auto b) { return a.z < b.z; });
	int previous_material = -1;
	int id = 0;
	svg << "<g stroke=\"" << process.outline_color << "\" stroke-width=\"" << process.outline_thickness << "\">";
	for (auto element : elements) {
		int need_change_material = previous_material != element.material;

		if (need_change_material) {
			if (previous_material != -1) {
				svg << "</g>";
			}
			previous_material = element.material;
			char color[16];
			/*if (element.material != -1) {
				get_diffuse_from_tinyobj_material(process, element.material, color);
			}
			else {*/
			strcpy(color, "#aaa");
			//}
			svg << "<g fill=\"" << color << "\">";
			id++;
		}
		svg << "<polygon points=\"";
		for (auto a : element.vertices) {
			svg << a.x * process.svg_scale << "," << a.y * process.svg_scale << " ";
		}
		svg << "\"/>";
	}
	svg << "</g></g></svg>";
}
