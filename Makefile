CC=gcc
CXX=g++
TARGET=Platformer
INC_FLAGS := -iquote submodules/imgui -I include -I build
CFLAGS := -O2 -march=native
CXXFLAGS := -O2 -march=native -fno-rtti -std=c++17
CPPFLAGS = $(INC_FLAGS) -DIMGUI_IMPL_OPENGL_LOADER_CUSTOM="<gl.h>" -MT $@ -MMD -MP -MF build/$*.d
IMGUI_SRCS := submodules/imgui/examples/imgui_impl_glfw.cpp submodules/imgui/examples/imgui_impl_opengl3.cpp $(shell find submodules/imgui -path "submodules/imgui/imgui*.cpp")
SRCS := build/gl.c $(shell find src -path "*.cpp") $(IMGUI_SRCS)
OBJS := $(patsubst %.c, ./build/%.o, $(patsubst %.cpp, ./build/%.o, $(SRCS)))
TEST_SRCS := $(shell find test -path "*.cpp")
TEST_OBJS := $(patsubst %.cpp, ./build/%.o, $(TEST_SRCS))
TEST_TARGETS := test/collisiontest test/collisiondemo test/mat2test test/rtreetest test/debugcollision
TEST_COMMON_OBJS := build/build/gl.o build/src/core/Map.o build/src/core/Editor.o build/src/GL/Helpers.o build/src/GL/SpriteBatchImpl.o \
build/src/util/Mat2D.o build/src/GL/Shader.o build/src/GL/Camera.o build/src/GL/TextureAtlas.o $(patsubst %.cpp, ./build/%.o, $(IMGUI_SRCS)) \
build/src/GL/Sprite.o build/src/GL/SpriteBatch.o
DEPS := $(OBJS:.o=.d) $(TEST_OBJS:.o=.d)

ifdef OS
	LDFLAGS=-lopengl32 -lglfw3 -lz -Wl,-O1 -mwindows -static-libstdc++ -static-libgcc -static
else
	LDFLAGS=-lGL -lglfw -lz -Wl,-O1
endif

.PHONY: all clean test

all: build build/gl.h $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $(TARGET) $(LDFLAGS)

./build/%.o : ./%.cpp build/gl.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

./build/%.o : ./%.c build/gl.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -fr $(TARGET) build $(TEST_TARGETS)

build/gl.c: build/gl.h
build/gl.h: build/galogen_exe
	cd build && ./galogen_exe ../submodules/galogen/third_party/gl.xml --api gl --ver 3.3 --profile core --exts EXT_texture_compression_s3tc,EXT_texture_sRGB,EXT_texture_filter_anisotropic --filename gl 

build/galogen_exe: build
	$(CXX) $(CXXFLAGS) submodules/galogen/galogen.cpp submodules/galogen/third_party/tinyxml2.cpp -o build/galogen_exe

build:
	mkdir build
	cd build && mkdir -p build src/core src/game src/GL src/util submodules/imgui/examples test

test: build build/gl.h $(TEST_OBJS) $(TEST_TARGETS)

test/collisiontest: $(TEST_COMMON_OBJS) build/test/offscreenWindow.o build/test/collisiontest.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

test/collisiondemo: $(TEST_COMMON_OBJS) build/src/GL/Window.o build/test/collisiondemo.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

test/mat2test: build/src/util/Mat2D.o build/test/mat2test.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

test/rtreetest: build/test/rtreetest.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

test/debugcollision: build/test/debugcollision.o build/build/gl.o build/src/GL/Helpers.o build/src/GL/TextureAtlas.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

-include $(DEPS)

