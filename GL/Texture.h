#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/Rect.hpp>
#include <gl.h>

struct Texture{
	Texture(GLuint *texture = nullptr, Rect<float> rect = Rect<float>(0.f,0.f,0.f,0.f)) : m_texture(texture), m_rect(rect){}
	GLuint* m_texture;
	Rect<float> m_rect;
	uint16_t width=0;
	uint16_t height=0;
	char rotated = 0;
};

#endif
