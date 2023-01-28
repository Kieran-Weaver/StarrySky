CC=gcc
CXX=g++ -Wall -Wextra -Wpedantic
TARGET=Platformer
INC_FLAGS := -iquote submodules/imgui -I include -I build
CFLAGS := -Og -g -march=native
CXXFLAGS := -Og -g -march=native -fno-rtti -std=c++17
CPPFLAGS = $(INC_FLAGS) -MT $@ -MMD -MP -MF build/$*.d
IMGUI_SRCS := submodules/imgui/backends/imgui_impl_glfw.cpp submodules/imgui/backends/imgui_impl_opengl3.cpp $(shell find submodules/imgui -path "submodules/imgui/imgui*.cpp")
SRCS := $(shell find src/core -path "*.cpp") $(shell find src/game -path "*.cpp") src/main.cpp
OBJS := $(patsubst %.cpp, ./build/%.o, $(SRCS))
LIB_SRCS := $(shell find src/GL -path "*.cpp") $(shell find src/util -path "*.cpp") $(shell find src/file -path "*.cpp") $(IMGUI_SRCS)
LIB_OBJS := $(patsubst %.c, ./build/%.o, $(patsubst %.cpp, ./build/%.o, $(LIB_SRCS)))
TEST_SRCS := $(shell find test -path "*.cpp")
TEST_OBJS := $(patsubst %.cpp, ./build/%.o, $(TEST_SRCS)) build/src/util/Mat2D.o \
build/src/file/PlainText.o build/src/core/RTree.o
TEST_TARGET := tests
TOOLS_TARGETS := tools/collisiondemo tools/debugcollision tools/packingtest
TOOLS_COMMON_OBJS := build/src/core/Map.o build/src/file/PlainText.o \
build/src/util/Mat2D.o build/src/GL/Shader.o build/src/GL/Camera.o build/src/GL/TextureAtlas.o \
build/src/GL/Sprite.o build/src/GL/Buffer.o build/src/GL/VertexArray.o build/src/GL/Program.o build/src/GL/Tilemap.o \
build/src/file/JSONReader.o build/src/file/JSONWriter.o build/src/core/RTree.o build/src/util/Clib.o
DEPS := $(OBJS:.o=.d) $(LIB_OBJS:.o=.d) $(TEST_OBJS:.o=.d)

ifdef OS
	LDFLAGS=-Wl,-O1 -static-libstdc++ -static-libgcc -static -lz
	GL_FLAGS=-lglbinding -lopengl32 -lglfw3 -mwindows
else
	LDFLAGS=-lz -Wl,-O1
	GL_FLAGS=-lglbinding -lGL -lglfw
endif

.PHONY: all clean test

all: build $(TARGET)
build:
	mkdir -p build
$(TARGET): $(OBJS) $(LIB_OBJS)
	$(CXX) $^ -o $(TARGET) $(GL_FLAGS) $(LDFLAGS)

./build/%.o : ./%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

./build/%.o : ./%.c
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -fr $(TARGET) build $(TEST_TARGET) $(TOOLS_TARGETS) offscreen.png tools/editor

build/libSSGL.a: lib

lib: $(LIB_OBJS)
	ar rcs build/libSSGL.a $(LIB_OBJS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

tools/debugcollision: build/test/debugcollision.o build/src/file/PlainText.o build/src/GL/TextureAtlas.o build/src/file/JSONReader.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(GL_FLAGS) $(LDFLAGS)

tools/collisiondemo: $(TOOLS_COMMON_OBJS) build/src/GL/Window.o build/tools/collisiondemo.o  build/src/GL/SpriteBatchImpl.o build/src/GL/SpriteBatch.o $(patsubst %.cpp, ./build/%.o, $(IMGUI_SRCS))
	$(CXX) $(CXXFLAGS) $^ -o $@ $(GL_FLAGS) $(LDFLAGS)

tools/packingtest: build/test/packingtest.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

tools/editor: build/tools/editor.o build/libSSGL.a
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(GL_FLAGS)

-include $(DEPS)

