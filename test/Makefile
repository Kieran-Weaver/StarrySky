CXX=g++
#CXXFLAGS=-Og -g -fsanitize=address,undefined -march=native -flto -std=c++11
CXXFLAGS=-I.. -I../rapidjson/include -I../parallel_hashmap -DIMGUI_IMPL_OPENGL_LOADER_CUSTOM="<gl.h>" -Og -g -Wall -Wextra -pedantic -march=native -std=c++11 -iquote ../imgui
OBJS=../gl.o ../GL/Window.o ../core/Map.o ../core/Editor.o ../GL/Helpers.o ../GL/SpriteBatchImpl.o ../GL/Mat2D.o ../GL/Shader.o ../GL/Camera.o ../GL/TextureAtlas.o ../imgui/imgui.o ../imgui/imgui_draw.o ../imgui/imgui_widgets.o ../imgui/imgui_demo.o ../imgui/examples/imgui_impl_glfw.o ../imgui/examples/imgui_impl_opengl3.o ../GL/Sprite.o ../GL/SpriteBatch.o
ifdef OS
	LDFLAGS=-lopengl32 -lglfw3 -lz -Wl,-O1 -mwindows -static-libstdc++ -static-libgcc -static
else
	LDFLAGS=-lGL -lglfw -lz -Wl,-O1
endif
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<
%.o : %.c
	$(CXX) $(CXXFLAGS) -o $@ -c $<
all: $(OBJS) collisiontest.o mat2test.o
	$(CXX) $(CXXFLAGS) $(OBJS) collisiontest.o -o collisiontest $(LDFLAGS)
	$(CXX) $(CXXFLAGS) mat2test.o -o mat2test $(OBJS) $(LDFLAGS)
clean:
	rm -fr $(OBJS) collisiontest.o mat2test.o collisiontest mat2test
