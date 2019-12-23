#ifndef TEXTURE_H
#define TEXTURE_H
#include "Rect.hpp"
#include "Bitmask.hpp"
struct Texture{
	Texture(GLuint texture = 0, Rect<uint16_t> rect = Rect<uint16_t>(0,0,0,0)) : m_texture(texture), m_rect(rect){}
	Texture(const Texture &texture){
		this->m_texture = texture.m_texture;
		this->m_rect = texture.m_rect;
		this->width = texture.width;
		this->height = texture.height;
		this->rotated = texture.rotated;
		this->m_bitmask = texture.m_bitmask;
	}
	GLuint m_texture = 0;
	Rect<uint16_t> m_rect;
	uint16_t width=0;
	uint16_t height=0;
	char rotated = 0;
	Bitmask m_bitmask;
};

#endif
