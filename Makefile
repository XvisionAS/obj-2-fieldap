BUILD_DIR = ./build
TARGET=obj-2-fieldap

SRCS=main.cpp \
	process/process_backface_culling.cpp \
	process/process_center_xy.cpp \
	process/process_compute_min_max.cpp \
	process/process_debug_render_mesh_to_tga.cpp \
	process/process_file_path_and_name.cpp \
	process/process_from_tinyobj_to_v3.cpp \
	process/process_generate_triangle_list.cpp \
	process/process_load_obj.cpp \
	process/process_optimize_mesh.cpp \
	process/process_output_socket.cpp \
	process/process_output_svg.cpp \
	process/process_output_threejs.cpp \
	process/process_remove_degenerate_triangle.cpp \
	process/process_split_triangle_by_material.cpp \
	process/process_swap_yz.cpp \
	process/process_transform_points.cpp \
	process/process_triangle_occlusion.cpp \
	process/process_glue_to_ground.cpp \
	externals/tiny_obj_loader.cc

CPP_OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)
OBJS=$(CPP_OBJS:%.cc=$(BUILD_DIR)/%.o)
DEPS=$(OBJS:%.o=%.d)
CPPFLAGS=-I. -g -O3 -Wall -Werror -Wextra
RM=rm -f

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX)  $(CPPFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS)

-include $(DEPS)

$(BUILD_DIR)/%.o : %.cpp
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/%.o : %.cc
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) -MMD -c $< -o $@

.PHONY: clean

clean:
	$(RM) $(OBJS) $(DEPS) $(TARGET) vgcore*

