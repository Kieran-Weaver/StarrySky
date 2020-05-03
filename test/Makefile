CC=gcc
CXX=g++
INC_FLAGS= -I.. -I../rapidjson/include -I../parallel_hashmap -DIMGUI_IMPL_OPENGL_LOADER_CUSTOM="<gl.h>" -iquote ../imgui
#CXXFLAGS=-Og -g -fsanitize=address,undefined -march=native -flto -std=c++11
CXXFLAGS=-O2 -g -Wall -Wextra -march=native -std=c++17

CPPFLAGS=$(INC_FLAGS) -DIMGUI_IMPL_OPENGL_LOADER_CUSTOM="<gl.h>" -MMD -MP

OBJS=../gl.o ../core/Map.o ../core/Editor.o ../GL/Helpers.o ../GL/SpriteBatchImpl.o \
../GL/Mat2D.o ../GL/Shader.o ../GL/Camera.o ../GL/TextureAtlas.o ../imgui/imgui.o \
../imgui/imgui_draw.o ../imgui/imgui_widgets.o ../imgui/imgui_demo.o \
../imgui/examples/imgui_impl_glfw.o ../imgui/examples/imgui_impl_opengl3.o \
../GL/Sprite.o ../GL/SpriteBatch.o   debugcollision.o collisiontest.o mat2test.o \
collisiondemo.o offscreenWindow.o rtreetest.o ../GL/Window.o

DEPS := $(OBJS:.o=.d)

COMMON_OBJS=../gl.o ../core/Map.o ../core/Editor.o ../GL/Helpers.o ../GL/SpriteBatchImpl.o \
../GL/Mat2D.o ../GL/Shader.o ../GL/Camera.o ../GL/TextureAtlas.o ../imgui/imgui.o \
../imgui/imgui_draw.o ../imgui/imgui_widgets.o ../imgui/imgui_demo.o \
../imgui/examples/imgui_impl_glfw.o ../imgui/examples/imgui_impl_opengl3.o \
../GL/Sprite.o ../GL/SpriteBatch.o

ifdef OS
	LDFLAGS=-lopengl32 -lglfw3 -lz -Wl,-O1 -mwindows -static-libstdc++ -static-libgcc -static
else
	LDFLAGS=-lGL -lglfw -lz -Wl,-O1
endif

.PHONY: all clean
all: collisiontest collisiondemo mat2test rtreetest debugcollision

collisiontest: $(OBJS)
	$(CXX) $(CXXFLAGS) $(COMMON_OBJS) offscreenWindow.o collisiontest.o -o collisiontest $(LDFLAGS)

collisiondemo: $(OBJS)
	$(CXX) $(CXXFLAGS) $(COMMON_OBJS) ../GL/Window.o collisiondemo.o -o collisiondemo $(LDFLAGS)

mat2test: $(OBJS)
	$(CXX) $(CXXFLAGS) ../GL/Mat2D.o mat2test.o -o mat2test $(LDFLAGS)

rtreetest: $(OBJS)
	$(CXX) $(CXXFLAGS) rtreetest.o -o rtreetest $(LDFLAGS)

debugcollision: $(OBJS)
	$(CXX) $(CXXFLAGS) debugcollision.o -o debugcollision $(LDFLAGS) ../gl.o ../GL/Helpers.o

clean:
	rm -fr $(OBJS) collisiontest collisiondemo mat2test debugcollision rtreetest *.o

-include $(DEPS)
