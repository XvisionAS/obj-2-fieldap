#include "process.h"
#include <list>
#include <set>

struct edge_t {
	edge_t(int a_, int b_, int m_)  : a(a_), b(b_), m(m_), count(0) {
	}

	int		a, b, m;
	int		count;

	bool operator<(const edge_t &that)  const {
		if (a != that.a) {
			return a < that.a;
		}
		else if (b != that.b) {
			return b < that.b;
		}
		else if (m != that.m) {
			return m < that.m;
		}
		return false;
	}
};

typedef std::set<edge_t> edge_infos_t;

struct element_t {
	int								material;
	float							z;
	size_t							id;
	std::vector<v3>									contour;
	std::vector<std::vector<v3> >		holes;
};

typedef std::vector<element_t> elements_t;

static	 void build_edge_list(process_t& process, edge_infos_t& edges) {
	std::map<edge_t, int>	edges_map;
	for (auto triangle : process.triangles) {
		if (triangle.valid) {
			auto addEdge = [&](int a, int b, int material) {
				if (a > b) {
					std::swap(a, b);
				}
				edges_map[edge_t(a, b, material)] ++;
			};
			addEdge(triangle.a, triangle.b, triangle.material);
			addEdge(triangle.b, triangle.c, triangle.material);
			addEdge(triangle.c, triangle.a, triangle.material);
		}
	}
	for (auto e : edges_map) {
		edge_t edge = e.first;
		edge.count = e.second;
		edges.insert(edge);
	}
	
}

static void remove_shared_edge(std::list<edge_t>& edges) {
	auto iter = edges.begin(), end = edges.end();
	while (iter != end) {
		if (iter->count > 1) {
			iter = edges.erase(iter);
		} else {
			++iter;
		}
	}
}

struct aabb {
	v3 	min;
	v3		max;
	int index;
	void init() {
		min.set(std::numeric_limits<float>::max());
		max.set(-std::numeric_limits<float>::max());
	}
	void add(const v3& v) {
		min.min(v);
		max.max(v);
	}

	void size(v3& v) const {
		v.x = fabsf(max.x - min.x);
		v.y = fabsf(max.y - min.y);
		v.z = fabsf(max.z - min.z);
	}
	
	bool bigger(const aabb& that) const {
		v3 a, b;
		
		size(a);
		that.size(b);
		return (a.x + a.y) > (b.x + b.y);
	}

	
};

static void build_element_list(process_t& process, edge_infos_t& edges, elements_t& elements) {
	
	
	while (!edges.empty()) {
		std::list<edge_t> stack_edge;		
		auto								iter = edges.begin();
		int								material = iter->m;
		int								to_find = iter->b;
		int								found = 1;
		
		stack_edge.push_back(*iter);
		edges.erase(iter);		
		while (found) {
			auto		it = std::find_if(edges.begin(), edges.end(), [=](auto& e) -> bool { return (to_find == e.a || to_find == e.b) && (e.m == material); });
			found = it != edges.end();
			if (found) {
				to_find = (it->a == to_find) ? it->b: it->a;
				stack_edge.push_back(*it);
				edges.erase(it);
			}
		}				

		remove_shared_edge(stack_edge);

		std::vector<std::list<int> >		stacks;
		std::vector<aabb	>							aabbs;
		while (!stack_edge.empty()) {
			stacks.push_back(std::list<int>());
			stacks.back().push_back(stack_edge.front().a);
			stacks.back().push_back(stack_edge.front().b);
			stack_edge.erase(stack_edge.begin());		
			found = 1;
			while (found) {
				to_find = stacks.back().back();
				auto		it = std::find_if(stack_edge.begin(), stack_edge.end(), [=](auto& e) -> bool { return to_find == e.a || to_find == e.b; });
				found = it != stack_edge.end();
				if (found) {
					stacks.back().push_back((it->a == to_find) ? it->b : it->a);
					stack_edge.erase(it);
				}
			}
			stacks.back().pop_back();

			aabbs.push_back(aabb());
			aabbs.back().index = (int)aabbs.size() - 1;
			aabbs.back().init();
			for (auto eit : stacks.back()) {
				aabbs.back().add(process.vertices[eit]);
			}
		}

		std::sort(aabbs.begin(), aabbs.end(), [](const auto& a, const auto& b) -> bool { return a.bigger(b); });
		if (!stacks.empty()) {
			elements.push_back(element_t());
			element_t& element = elements.back();
			element.id = elements.size();
			element.material = material;
			element.z = -std::numeric_limits<float>::max();
			
			for (int index = 0; index < aabbs.size(); ++index) {
				if (index != 0) {
					element.holes.push_back(std::vector<v3>());
				}
				for (auto eit : stacks[aabbs[index].index]) {
					const auto& a = process.vertices[eit];					
					if (index == 0) {
						element.contour.push_back(a);
						element.z = std::max(element.z, a.z);
					} else {
						element.holes.back().push_back(a);
					}
				}		
			}
		}	
	}
}

static void get_diffuse_from_tinyobj_material(process_t& process, int material, char* buffer) {
	float *diffuse = process.tinyobj_materials[material].diffuse;
	int			diffuse_as_int[3] = {
		(int)(diffuse[0] * 255),
		(int)(diffuse[1] * 255),
		(int)(diffuse[2] * 255),
	};
	sprintf(buffer, "#%.2X%.2X%.2X", diffuse_as_int[0], diffuse_as_int[1], diffuse_as_int[2]);
}

void process_output_svg(process_t& process) {
	edge_infos_t		edges;

	build_edge_list(process, edges);	
	
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
		if (element.holes.empty()) {
			svg << "<polygon points=\"";
			for (auto a : element.contour) {
				svg << a.x << "," << a.y << " ";
			}
			svg << "\"/>";
			svg << "<!-- z === " << element.z << "-->";
		}
		else {
			auto outputPath = [](auto& stream, auto& vertices) {
				stream << "M " << vertices[0].x << " " << vertices[0].y;
				for (auto i = 1; i < vertices.size(); ++i) {
					stream << " L " << vertices[i].x << " " << vertices[i].y;
				}
				stream << " Z";
			};

			svg << "<path d=\"";
			outputPath(svg, element.contour);
			svg << "\"/>";
		}
	}
	svg << "</g></svg>";
}
