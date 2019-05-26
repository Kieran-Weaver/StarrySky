#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/Rect.hpp>
#include <gl31.h>

struct Texture{
	Texture(GLuint *texture = nullptr, Rect<float> rect = Rect<float>(0.f,0.f,0.f,0.f)) : m_texture(texture), m_rect(rect){}
	GLuint* m_texture;
	Rect<float> m_rect;
};

#endif
