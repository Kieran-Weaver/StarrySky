CXX=g++
#CXXFLAGS=-DUSE_ZLIB -Og -g -fsanitize=address,undefined -march=native -std=c++11
CXXFLAGS=-DIMGUI_IMPL_OPENGL_LOADER_CUSTOM="<gl.h>" -Og -g -Wall -Wextra -pedantic -march=native -std=c++11 -I. -iquote imgui
OBJS=main.o gl.o GL/Helpers.o GL/Shader.o GL/Camera.o GL/TextureAtlas.o imgui/imgui.o imgui/imgui_draw.o imgui/imgui_widgets.o imgui/imgui_demo.o imgui/examples/imgui_impl_glfw.o imgui/examples/imgui_impl_opengl3.o
LDFLAGS=-lGL -lglfw -ldl -lz
all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o Platformer $(LDFLAGS)
clean:
	rm -fr $(OBJS) Platformer
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<
%.o : %.c
	$(CXX) $(CXXFLAGS) -o $@ -c $<
