#ifndef STARRYSKY_SPRITEDATA_HPP
#define STARRYSKY_SPRITEDATA_HPP
#include <array>
#include <cstdint>
struct __attribute__((packed)) SpriteData{
	std::array<uint16_t, 3> texpos = {0, 0, 0};
	std::array<float,3> vtxPos = {0.f, 0.f, 0.f};
	std::array<uint8_t,4> sprColor = { 255, 255, 255, 255 };
};
#endif
