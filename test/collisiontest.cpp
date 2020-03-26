#include <iostream>
#include <GL/TextureAtlas.hpp>
#include <GL/SpriteBatch.hpp>
#include <GL/Camera.hpp>
#include <GL/Window.hpp>
#include <GL/Sprite.hpp>
#include <GLFW/glfw3.h>
#include <zlib.h>
static unsigned char* my_stbi_zlib_compress( unsigned char *data, int data_len, int *out_len, int quality );
#define STBIW_ZLIB_COMPRESS my_stbi_zlib_compress
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#define WIDTH 720
#define HEIGHT 640
static unsigned char* my_stbi_zlib_compress( unsigned char *data, int data_len, int *out_len, int quality ){
	uLongf buflen = compressBound(data_len);
	unsigned char* buf = (unsigned char*)malloc(buflen);
	if( buf == NULL || compress2(buf, &buflen, data, data_len, quality) != 0 ){
		free(buf);
		return NULL;
	}
	*out_len = buflen;
	return buf;
}
int main(int, char const**) {
	char* buffer = new char[4 * WIDTH * HEIGHT];
	float x1=247,y1=362,x2=500,y2=200;
	Window window(WIDTH, HEIGHT, 3, 3, "data/fonts/boxfont_round.ttf", "Collision Test");
	Camera camera(Rect<float>(-2000.f,-2000.f,4000.f,4000.f),Rect<float>(0.f,0.f,800.f,400.f), window);
	WindowState ws;
	ws.camera = &camera;
	window.getWindowState() = ws;
	TextureAtlas atlas("data/atlas.json");
	SpriteBatch batch(atlas, "data/shaders.json");
	Texture t = atlas.findSubTexture("test1");
	Texture t2 = atlas.findSubTexture("test2");
	Sprite s;
	s.setTexture(t);
	Sprite s2;
	s2.setTexture(t2);
	s.setPosition(x1,y1);
	s2.setPosition(x2,y2);
	s2.rotate(0.0);
	s.rotate(-1.6);
	bool collided = s.PPCollidesWith(s2);
	window.startFrame();
	window.makeCurrent();
	batch.Draw(s);
	batch.Draw(s2);
	batch.Draw(window);
	window.endFrame();
	glFinish();
	glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	stbi_write_png("offscreen.png", WIDTH, HEIGHT, 4,buffer + (WIDTH * 4 * (HEIGHT - 1)),-WIDTH* 4);
	delete[] buffer;
	assert(!collided);
	return 0;
}
