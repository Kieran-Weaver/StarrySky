CXX=g++
#CXXFLAGS=-Og -g -fsanitize=address,undefined -march=native -flto -std=c++11
CXXFLAGS=-I.. -DIMGUI_IMPL_OPENGL_LOADER_CUSTOM="<gl.h>" -O2 -Wall -Wextra -pedantic -march=native -std=c++11 -I. -iquote ../imgui
OBJS=../gl.o ../GL/Helpers.o ../GL/Shader.o ../GL/Camera.o ../GL/TextureAtlas.o collisiontest.o ../imgui/imgui.o ../imgui/imgui_draw.o ../imgui/imgui_widgets.o ../imgui/imgui_demo.o ../imgui/examples/imgui_impl_glfw.o ../imgui/examples/imgui_impl_opengl3.o ../GL/Sprite.o ../GL/SpriteBatch.o
LDFLAGS=-lGL -lglfw -ldl -lz
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<
%.o : %.c
	$(CXX) $(CXXFLAGS) -o $@ -c $<
all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o collisiontest $(LDFLAGS)
clean: $(OBJS)
	rm -fr $(OBJS) collisiontest
