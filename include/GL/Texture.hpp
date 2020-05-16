#ifndef TEXTURE_H
#define TEXTURE_H
#include <util/Rect.hpp>
#include <util/Bitmask.hpp>
#include <gl.h>
struct Texture{
	GLuint m_texture = 0;
	Rect<uint16_t> m_rect = Rect<uint16_t>(0,0,0,0);
	uint16_t width=0;
	uint16_t height=0;
	char rotated = 0;
	Bitmask m_bitmask;
};

#endif
