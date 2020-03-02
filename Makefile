CC=gcc
CXX=g++
TARGET=Platformer
CFLAGS := -O2 -march=native -I. -iquote imgui
CXXFLAGS := -O2 -march=native -fno-rtti -std=c++11 -I. -iquote imgui -Irapidjson/include
CPPFLAGS := $(INC_FLAGS) -DIMGUI_IMPL_OPENGL_LOADER_CUSTOM="<gl.h>" -MMD -MP
IMGUI_SRCS := imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp imgui/imgui_demo.cpp imgui/examples/imgui_impl_glfw.cpp imgui/examples/imgui_impl_opengl3.cpp
SRCS := gl.c $(shell find . -path "./GL/*.cpp" -o -path "./core/*.cpp" -o -path "./game/*.cpp" ) main.cpp $(IMGUI_SRCS)
OBJS := $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)
ifdef OS
	LDFLAGS=-lopengl32 -lglfw3 -lz -Wl,-O1 -mwindows -static-libstdc++ -static-libgcc -static
	CPPFLAGS += -I/mingw64/include/GLFW
else
	LDFLAGS=-lGL -lglfw -lz -Wl,-O1
#	CPPFLAGS += -I/usr/include/GLFW
endif
all: gl.h $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -fr $(OBJS) $(TARGET) $(DEPS) gl.c gl.h galogen_exe

gl.c: gl.h
gl.h: galogen_exe
	./galogen_exe galogen/third_party/gl.xml --api gl --ver 3.3 --profile core --exts EXT_texture_compression_s3tc,EXT_texture_sRGB,EXT_texture_filter_anisotropic --filename gl 

galogen_exe: 
	$(CXX) $(CXXFLAGS) galogen/galogen.cpp galogen/third_party/tinyxml2.cpp -o galogen_exe

-include $(DEPS)

