#include "process.h"

struct edge_info_t {
	int material;
	int count;
};

typedef std::pair<int, int>						edge_t;
typedef std::map<edge_t, edge_info_t> edge_infos_t;

struct element_t {
	int								material;
	float							z;
	size_t							id;
	std::vector<int>		indices;
	std::vector<v3>		vertices;
};

typedef std::vector<element_t> elements_t;

static	 void build_edge_list(process_t& process, edge_infos_t& edges) {
	for (auto triangle : process.triangles) {
		if (triangle.valid) {
			auto addEdge = [&](int a, int b, int material) {
				if (a > b) {
					std::swap(a, b);
				}
				auto& e = edges[edge_t(a, b)];
				e.count++;
				e.material = material;
			};
			addEdge(triangle.a, triangle.b, triangle.material);
			addEdge(triangle.b, triangle.c, triangle.material);
			addEdge(triangle.c, triangle.a, triangle.material);
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

		int material = iter->second.material;
		stack.push_back(iter->first.first);
		stack.push_back(iter->first.second);
		edges.erase(iter);

		int found = 1;
		while (found) {
			int		to_find = stack.back();
			
			auto		it = std::find_if(edges.begin(), edges.end(), [=](auto& e) -> bool { return (to_find == e.first.first || to_find == e.first.second) && (e.second.material == material); });
			found = it != edges.end();
			if (found) {
				stack.push_back(it->first.first == to_find ? it->first.second : it->first.first);
				edges.erase(it);
			}
		}
		stack.pop_back();

		elements.push_back(element_t());
		element_t& element = elements.back();
		element.id = elements.size();
		element.material = material;
		element.z = -std::numeric_limits<float>::max();
		for (auto eit : stack) {
			const auto& a = process.vertices[eit];
			element.indices.push_back(eit);
			element.vertices.push_back(a);
			element.z = std::max(element.z, a.z);
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
	svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"" << process.min.x << " " << process.min.y << " " << (process.max.x - process.min.x) << " " << (process.max.y - process.min.y) << "\" >";

	std::sort(elements.begin(), elements.end(), [](auto a, auto b) { return a.z < b.z; });
	int previous_material = -1;
	int id = 0;
	for (auto element : elements) {
		int need_change_material = previous_material != element.material;

		if (need_change_material) {
			if (previous_material != -1) {
				svg << "</g>";
			}
			previous_material = element.material;
			char color[16];
			get_diffuse_from_tinyobj_material(process, element.material, color);
			svg << "<g id=\"" << id << "\" stroke=\"black\" fill=\"" << color << "\" stroke-width=\"0.01\">";
			id++;
		}
		svg << "<polygon points=\"";
		for (auto a : element.vertices) {
			svg << a.x << "," << a.y << " ";
		}
		svg << "\"/>";
	}
	svg << "</g></svg>";
}
