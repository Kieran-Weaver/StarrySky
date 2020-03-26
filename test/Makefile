CXX=g++
#CXXFLAGS=-Og -g -fsanitize=address,undefined -march=native -flto -std=c++11
CXXFLAGS=-I.. -I../rapidjson/include -I../parallel_hashmap -DIMGUI_IMPL_OPENGL_LOADER_CUSTOM="<gl.h>" -O2 -g -Wall -Wextra -march=native -std=c++11 -iquote ../imgui
OBJS=../gl.o ../core/Map.o ../core/Editor.o ../GL/Helpers.o ../GL/SpriteBatchImpl.o ../GL/Mat2D.o ../GL/Shader.o ../GL/Camera.o ../GL/TextureAtlas.o ../imgui/imgui.o ../imgui/imgui_draw.o ../imgui/imgui_widgets.o ../imgui/imgui_demo.o ../imgui/examples/imgui_impl_glfw.o ../imgui/examples/imgui_impl_opengl3.o ../GL/Sprite.o ../GL/SpriteBatch.o
ifdef OS
	LDFLAGS=-lopengl32 -lglfw3 -lz -Wl,-O1 -mwindows -static-libstdc++ -static-libgcc -static
else
	LDFLAGS=-lGL -lglfw -lz -Wl,-O1
endif
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<
%.o : %.c
	$(CXX) $(CXXFLAGS) -o $@ -c $<
all: $(OBJS)  debugcollision.o collisiontest.o mat2test.o collisiondemo.o offscreenWindow.o rtreetest.o ../GL/Window.o
	$(CXX) $(CXXFLAGS) $(OBJS) offscreenWindow.o collisiontest.o -o collisiontest $(LDFLAGS)
	$(CXX) $(CXXFLAGS) $(OBJS) ../GL/Window.o collisiondemo.o -o collisiondemo $(LDFLAGS)
	$(CXX) $(CXXFLAGS) ../GL/Mat2D.o mat2test.o -o mat2test $(LDFLAGS)
	$(CXX) $(CXXFLAGS) rtreetest.o -o rtreetest $(LDFLAGS)
	$(CXX) $(CXXFLAGS) debugcollision.o -o debugcollision $(LDFLAGS) ../gl.o ../GL/Helpers.o
clean:
	rm -fr $(OBJS) collisiontest collisiondemo mat2test debugcollision *.o
