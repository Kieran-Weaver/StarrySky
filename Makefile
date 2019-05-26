CXX=g++
#CXXFLAGS=-DUSE_ZLIB -Og -g -fsanitize=address,undefined -march=native -std=c++11
CXXFLAGS=-DUSE_ZLIB -Og -g -Wall -Wextra -pedantic -march=native -std=c++11 -I.
OBJS=main.o gl31.o GL/Helpers.o GL/Shader.o GL/Camera.o GL/TextureAtlas.o
LDFLAGS=-lGL -lglfw -ldl -lz
all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o Platformer $(LDFLAGS)
clean:
	rm -fr $(OBJS) Platformer
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<
%.o : %.c
	$(CXX) $(CXXFLAGS) -o $@ -c $<
