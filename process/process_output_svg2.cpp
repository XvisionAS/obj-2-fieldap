//#include "process.h"
//#include <set>
//
//
//struct edge2_t {
//	edge2_t(int a_, int b_, int m_) : a(a_), b(b_), m(m_), shared(0) {
//	}
//
//	int						a, b, m;
//	mutable int		shared;
//
//	bool operator<(const edge2_t &that)  const {
//		if (a != that.a) {
//			return a < that.a;
//		}
//		else if (b != that.b) {
//			return b < that.b;
//		}
//		else if (m != that.m) {
//			return m < that.m;
//		}
//		return false;
//	}
//};
//
//template <typename T>
//using array = std::vector<T>;
//
//typedef std::set<edge2_t>				edge_infos_t;
//typedef array<edge2_t>						edges_t;
//typedef array<v3>								vertices_t;
//
//struct element2_t {
//	int								material;
//	float							z;
//	size_t						id;	
//	edges_t						edges;
//	vertices_t				contour;
//	array<vertices_t>	holes;
//};
//
//typedef std::vector<element2_t> elements2_t;
//
//static	 void build_edge_list(process_t& process, edge_infos_t& edges) {
//	for (auto triangle : process.triangles) {
//		if (triangle.valid) {
//			auto addEdge = [&](int a, int b, int material) {
//				if (a > b) {
//					std::swap(a, b);
//				}
//				edge2_t edge(a, b, material);
//				auto e = edges.find(edge);
//				if (e != edges.end()) {
//					e->shared++;
//				} else {
//					edges.insert(edge);
//				}
//			};
//			addEdge(triangle.a, triangle.b, triangle.material);
//			addEdge(triangle.b, triangle.c, triangle.material);
//			addEdge(triangle.c, triangle.a, triangle.material);
//		}
//	}
//}
//
//
//
//static void remove_shared_edge(edges_t& edges) {
//	for (size_t i = 0; i < edges.size(); ) {
//		auto& e = edges[i];
//		if (e.shared != 0) {
//			e =  edges.back();
//			edges.pop_back();
//		}
//		else {
//			++i;
//		}
//	}
//}
//
//template<typename T>
//static int get_next_shared_edges(T& from, edges_t& next) {
//	if (from.empty()) {
//		return 0;
//	}
//
//	auto first = from.begin();
//	next.push_back(*first);
//	size_t	to_find = first->b;
//	int			material = first->m;
//	from.erase(first);
//
//	bool		found = true;
//
//	while (found) {
//		auto		it = std::find_if(from.begin(), from.end(),
//			[=](auto& e) -> bool { return (to_find == e.a || to_find == e.b) && (e.m == material); }
//		);
//		found = it != from.end();
//		if (found) {
//			to_find = (it->a == to_find) ? it->b : it->a;
//			next.push_back(*it);
//			from.erase(it);
//		}
//	}
//	return material;
//}
//
//static void build_element_edge_list(process_t& process, edge_infos_t& edges, elements2_t& elements) {
//	auto iter = edges.begin(), end = edges.end();
//	while (!edges.empty()) {		
//		elements.push_back(element2_t());
//		elements.back().id = elements.size();
//		elements.back().material = get_next_shared_edges(edges, elements.back().edges);
//	}
//}
//
//static void build_vertices_from_edges(process_t& process, edges_t& edges, vertices_t& v3, bool reverse) {
//	auto it = edges.begin();
//	
//	if (reverse) {
//		v3.push_back(process.vertices[it->a]);
//		v3.push_back(process.vertices[it->b]);
//	}
//	else {
//		v3.push_back(process.vertices[it->b]);
//		v3.push_back(process.vertices[it->a]);
//	}
//
//	int previous = it->b;
//	
//	for (++it; it != edges.end(); ++it) {
//		previous = (previous == it->b) ? it->a : it->b;
//		v3.push_back(process.vertices[previous]);
//	}
//	
//}
//
//static void build_element_list(process_t& process, elements2_t& elements) {
//
//	for (auto& element : elements) {
//		edges_t edges;
//		// remove_shared_edge(element.edges);
//		if (!element.edges.empty()) {
//			// get_next_shared_edges(element.edges, edges);
//
//			// element.z = build_vertices_from_edges(process, element.edges, element.contour, false);
//			//while (!element.edges.empty()) {
//			//	edges_t hole_edges;
//			//	get_next_shared_edges(element.edges, hole_edges);
//			//	if (hole_edges.size() > 2) {
//			//		element.holes.push_back(vertices_t());
//			//		build_vertices_from_edges(process, edges, element.holes.back(), true);
//			//	}
//			//}
//		}
//	}
//}
//
//static void get_diffuse_from_tinyobj_material(process_t& process, int material, char* buffer) {
//	float *diffuse = process.tinyobj_materials[material].diffuse;
//	float actual_diffuse[] = {
//		diffuse[0], diffuse[1], diffuse[2]
//	};
//
//	if (process.use_gamma_correction) {
//		actual_diffuse[0] = pow(actual_diffuse[0], 1.0f / process.gamma);
//		actual_diffuse[1] = pow(actual_diffuse[1], 1.0f / process.gamma);
//		actual_diffuse[2] = pow(actual_diffuse[2], 1.0f / process.gamma);
//	}
//	actual_diffuse[0] = std::min(1.0f, std::max(0.0f, actual_diffuse[0]));
//	actual_diffuse[1] = std::min(1.0f, std::max(0.0f, actual_diffuse[1]));
//	actual_diffuse[2] = std::min(1.0f, std::max(0.0f, actual_diffuse[2]));
//
//	int			diffuse_as_int[3] = {
//		(int)(actual_diffuse[0] * 255),
//		(int)(actual_diffuse[1] * 255),
//		(int)(actual_diffuse[2] * 255),
//	};
//	sprintf(buffer, "#%.2X%.2X%.2X", diffuse_as_int[0], diffuse_as_int[1], diffuse_as_int[2]);
//}
//
//void process_output_svg2(process_t& process) {
//	edge_infos_t		edges;
//
//	build_edge_list(process, edges);	
//	
//	elements2_t elements;
//	build_element_edge_list(process, edges, elements);	
//	build_element_list(process, elements);
//
//	std::ofstream svg;
//	svg.open(process.file_name_without_ext + ".second.svg");
//	svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"" << process.min.x << " " << process.min.y << " " << (process.max.x - process.min.x) << " " << (process.max.y - process.min.y) << "\" >";
//
//	std::sort(elements.begin(), elements.end(), [](auto a, auto b) { return a.z < b.z; });
//	int previous_material = -1;
//	int id = 0;
//	for (auto element : elements) {
//		int need_change_material = previous_material != element.material;
//
//		if (need_change_material) {
//			if (previous_material != -1) {
//				svg << "</g>";
//			}
//			previous_material = element.material;
//			char color[16];
//			get_diffuse_from_tinyobj_material(process, element.material, color);
//			svg << "<g id=\"" << id << "\" stroke=\"black\" fill=\"" << color << "\" stroke-width=\"0.01\">";
//			id++;
//		}
//		//if (element.holes.empty()) {
//		//	svg << "<polygon points=\"";
//		//	for (auto a : element.contour) {
//		//		svg << a.x << "," << a.y << " ";
//		//	}
//		//	svg << "\"/>";
//		//	svg << "<!-- z === " << element.z << "-->";
//		//}
//		//else {
//			auto outputPath = [](auto& stream, auto& vertices) {
//				stream << "M " << vertices[0].x << " " << vertices[0].y;
//				for (size_t i = 1; i < vertices.size(); ++i) {
//					stream << " L " << vertices[i].x << " " << vertices[i].y;
//				}
//				stream << " Z";
//			};
//
//			if (!element.contour.empty()) {
//				svg << "<path d=\"";
//				outputPath(svg, element.contour);
//				svg << "\"/>";
//				for (auto& hole : element.holes) {
//					svg << "<path d=\"";
//					outputPath(svg, hole);
//					svg << "\"/>";
//				}
//			}
//
//		// }
//	}
//	svg << "</g></svg>";
//}
