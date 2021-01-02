#ifndef STARRYSKY_TEXTURE_HPP
#define STARRYSKY_TEXTURE_HPP
#include <util/Rect.hpp>
#include <util/Bitmask.hpp>
struct Texture{
	uint32_t m_texture = 0;
	Rect<uint16_t> m_rect = {0,0,0xFFFF,0xFFFF};
	uint16_t width=0;
	uint16_t height=0;
	uint32_t type = 0;
	char rotated = 0;
	Bitmask m_bitmask;
};

#endif
