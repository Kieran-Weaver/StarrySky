CC=gcc
CXX=g++
LD=g++
TARGET=Platformer
CFLAGS := -O2 -Wall -Wextra -pedantic -march=native -I. -iquote imgui
CXXFLAGS := -O2 -Wall -Wextra -pedantic -march=native -std=c++11 -I. -iquote imgui
CPPFLAGS := $(INC_FLAGS) -DIMGUI_IMPL_OPENGL_LOADER_CUSTOM="<gl.h>" -MMD -MP
IMGUI_SRCS := imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp imgui/imgui_demo.cpp imgui/examples/imgui_impl_glfw.cpp imgui/examples/imgui_impl_opengl3.cpp
SRCS := $(shell find . -path "./GL/*.cpp" -o -path "./core/*.cpp" -o -path "./game/*.cpp" ) main.cpp gl.c $(IMGUI_SRCS)
OBJS := $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)
LDFLAGS=-lopengl32 -lglfw3 -lz -Wl,-O1
all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -fr $(OBJS) $(TARGET) $(DEPS)
	
-include $(DEPS)